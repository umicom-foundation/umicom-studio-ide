/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ui/resources/icon.c
 *
 * PURPOSE:
 *   Provide a *pure-C* implementation of tiny icon helpers:
 *     - create a GtkImage widget for our brand/logo
 *     - produce a GdkPixbuf fallback (always available via XPM)
 *     - optionally stick the logo into the headerbar (left side)
 *     - placeholder for a Win32 taskbar icon hook (GTK4 lacks API)
 *
 * WHY PURE-C (no XML/gresource):
 *   - Keeps the build simple (no codegen steps).
 *   - Works great for small assets and lets new contributors read/modify
 *     everything in plain C without learning another tool.
 *
 * LOOSE COUPLING:
 *   - This file has *no* dependencies on other Umicom modules.
 *   - Only GTK/GdkPixbuf are included. Any GUI code can use it freely.
 *
 * BEGINNER NOTE:
 *   - XPM is an old text format that can be embedded as a C array. GTK can
 *     turn that text into a `GdkPixbuf` at runtime using
 *     `gdk_pixbuf_new_from_xpm_data()`. It isn’t fancy, but it’s tiny and
 *     avoids external files.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-14 | MIT
 *---------------------------------------------------------------------------*/

#include "icon.h"                  /* Public declarations for this module.     */
#include <string.h>                /* For memset/memcpy if we expand later.    */

/*-----------------------------------------------------------------------------
 * SECTION: tiny XPM fallback
 *
 * We embed a super-simple 16x16 and 32x32 glyph that echoes the Umicom style
 * (dark slate “u/angle” mark). These are *placeholders* until you decide to
 * swap in richer PNG/SVG bytes (we can add a loader later and keep fallback).
 *---------------------------------------------------------------------------*/

/* 16x16 XPM: palette with transparency (.) and two tones (#, *).              */
static const char* s_umicom16_xpm[] = {
  /* width height ncolors chars_per_pixel */
  "16 16 3 1",
  /* colors: '.' transparent, '#' dark line, '*' accent */
  ". c None",
  "# c #1E2A35",
  "* c #C0392B",
  /* pixels (each line is one image row) */
  "................",
  ".....####.......",
  "....######......",
  "...###..###.....",
  "..###....###....",
  ".###......###...",
  ".###......###...",
  ".###..**..###...",
  ".###.****.###...",
  ".###..**..###...",
  ".###......###...",
  "..###....###....",
  "...###..###.....",
  "....######......",
  ".....####.......",
  "................",
};

/* 32x32 XPM: same motif, a little smoother at larger size.                    */
static const char* s_umicom32_xpm[] = {
  "32 32 3 1",
  ". c None",
  "# c #1E2A35",
  "* c #C0392B",
  "................................",
  "..........######................",
  ".........########...............",
  "........###....###..............",
  ".......###......###.............",
  "......###........###............",
  ".....###..........###...........",
  ".....###....**....###...........",
  "....###....****....###..........",
  "....###....****....###..........",
  "....###....**......###..........",
  ".....###..........###...........",
  ".....###..........###...........",
  "......###........###............",
  ".......###......###.............",
  "........###....###..............",
  ".........########...............",
  "..........######................",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
};

/*-----------------------------------------------------------------------------
 * helper_xpm_for_size
 *
 * PURPOSE:
 *   Pick the “best” embedded XPM for the requested size. We keep this tiny
 *   and deterministic: <=20px -> 16x16, else -> 32x32.
 *---------------------------------------------------------------------------*/
static const char* const* helper_xpm_for_size (int size_px) {
  /* Defensive clamp: negative or zero sizes make no sense; normalize to 16.  */
  if (size_px <= 0) size_px = 16;
  /* Choose the nearest small or large fallback.                               */
  return (size_px <= 20) ? s_umicom16_xpm : s_umicom32_xpm;
}

/*-----------------------------------------------------------------------------
 * umi_icon_pixbuf_fallback
 *
 * PURPOSE:
 *   Build a GdkPixbuf from the embedded XPM. We always succeed because XPM
 *   parsing is built into GdkPixbuf and our text is bundled in this file.
 *
 * RETURNS:
 *   New reference to GdkPixbuf (caller must g_object_unref()).
 *---------------------------------------------------------------------------*/
GdkPixbuf* umi_icon_pixbuf_fallback (int size_px) {
  /* Pick the closest XPM by size.                                             */
  const char* const* xpm = helper_xpm_for_size(size_px);
  /* Convert XPM text -> GdkPixbuf. This never returns NULL for valid XPM.     */
  GdkPixbuf* pb = gdk_pixbuf_new_from_xpm_data((const char**)xpm);
  /* Return the pixbuf to caller (they own one reference).                      */
  return pb;
}

/*-----------------------------------------------------------------------------
 * umi_icon_make_logo_image
 *
 * PURPOSE:
 *   Create a GtkImage from our fallback pixbuf and ask GTK to scale it to
 *   approximately `px` size. We keep the scaling simple; for crisp branding
 *   you can later swap this fallback with higher-res PNG bytes.
 *---------------------------------------------------------------------------*/
GtkWidget* umi_icon_make_logo_image (int px) {
  /* Step 1: obtain a pixbuf (always succeeds thanks to the XPM fallback).     */
  GdkPixbuf* pb = umi_icon_pixbuf_fallback(px);
  /* Step 2: if scaling is desired, let GdkPixbuf do it here.                  */
  int w  = gdk_pixbuf_get_width(pb);   /* read current width of the pixbuf.    */
  int h  = gdk_pixbuf_get_height(pb);  /* read current height of the pixbuf.   */
  int sz = (px > 0) ? px : w;          /* clamp px; default to current width.   */

  /* If the current size already matches, reuse; otherwise make a scaled copy. */
  GdkPixbuf* scaled = (w == sz || h == sz)
                      ? g_object_ref(pb)                                     /* keep */
                      : gdk_pixbuf_scale_simple(pb, sz, sz, GDK_INTERP_BILINEAR);

  /* Wrap the (possibly scaled) pixbuf into a GtkImage widget for packing.     */
  GtkWidget* img = gtk_image_new_from_pixbuf(scaled);

  /* Drop our references: GtkImage holds its own reference internally.         */
  g_object_unref(scaled);
  g_object_unref(pb);

  /* Return the ready-to-use GtkImage to the caller.                            */
  return img;
}

/*-----------------------------------------------------------------------------
 * find_headerbar_for_window
 *
 * PURPOSE:
 *   Internal utility to fetch the header bar (GtkHeaderBar) of a given window.
 *   In modern GTK4 templates you typically set a titlebar on the window; if it
 *   isn’t present we just return NULL and the caller does nothing.
 *---------------------------------------------------------------------------*/
static GtkWidget* find_headerbar_for_window (GtkWindow* win) {
  /* Convert window to generic widget so we can query “titlebar” child.         */
  GtkWidget* w = GTK_WIDGET(win);
  /* GTK4 API: gtk_window_get_titlebar() returns a GtkWidget* or NULL.         */
  GtkWidget* titlebar = gtk_window_get_titlebar(win);
  /* If there is no custom titlebar, give up quietly.                           */
  if (!titlebar) return NULL;
  /* Return the widget (usually a GtkHeaderBar).                                */
  return titlebar;
}

/*-----------------------------------------------------------------------------
 * umi_icon_try_apply_headerbar_logo
 *
 * PURPOSE:
 *   Create a small logo image and pack it at the *start* of the header bar.
 *   We do *not* replace any existing title widgets; we only add our icon
 *   to the typical “start” container so it plays nicely with other content.
 *---------------------------------------------------------------------------*/
void umi_icon_try_apply_headerbar_logo (GtkWindow* win, int desired_px) {
  /* Fetch the headerbar widget; if none, we simply return (no crash).         */
  GtkWidget* hb = find_headerbar_for_window(win);
  if (!hb) return;

  /* Build the image widget; choose a reasonable default if size not given.     */
  int px = (desired_px > 0) ? desired_px : 16;
  GtkWidget* img = umi_icon_make_logo_image(px);

  /* GTK4 header bars are GtkHeaderBar; they offer “pack-start” / “pack-end”.   */
  /* For strict typing, we cast to GtkHeaderBar (safe when template uses it).   */
  if (GTK_IS_HEADER_BAR(hb)) {
    /* Pack our image on the left side (“start”).                               */
    gtk_header_bar_pack_start(GTK_HEADER_BAR(hb), img);
    /* Make sure it becomes visible; many GTK containers don’t auto-map.        */
    gtk_widget_set_visible(img, TRUE);
  } else {
    /* If some custom widget is used as titlebar, we still add the icon as a
     * first child if the container is a GtkBox; if not, we do nothing.        */
    if (GTK_IS_BOX(hb)) {
      gtk_box_prepend(GTK_BOX(hb), img);
      gtk_widget_set_visible(img, TRUE);
    }
  }
}

/*-----------------------------------------------------------------------------
 * umi_icon_try_apply_taskbar_icon_win32
 *
 * PURPOSE:
 *   Placeholder for a future Windows-only enhancement. GTK4 removed the old
 *   `gtk_window_set_icon()` API; taskbar icons are typically taken from the
 *   executable resources. It is *possible* to poke the HWND via GDK, but that
 *   requires extra conversion code (pixbuf -> HICON). We keep this stub so
 *   callers can use it without #ifdefs; currently it safely does nothing.
 *---------------------------------------------------------------------------*/
void umi_icon_try_apply_taskbar_icon_win32 (GtkWindow* win) {
  (void)win; /* unused for now; the cast silences “unused parameter” warnings. */
  /* No-op today. If you later want us to implement the HWND path, we can add
   * a guarded Win32 block here that translates our pixbuf into an HICON and
   * calls SendMessageW(hwnd, WM_SETICON, ICON_BIG/ICON_SMALL, (LPARAM)hicon). */
}
/*---------------------------------------------------------------------------*/


