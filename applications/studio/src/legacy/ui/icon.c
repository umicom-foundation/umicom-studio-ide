/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ui/icon.c
 *
 * PURPOSE (What does this file do?):
 *   Provide tiny branding helpers for the Umicom Studio IDE. We embed a very
 *   small PNG (a 1×1 transparent pixel) so the code compiles and runs without
 *   any external asset files. Callers can obtain:
 *     - the raw PNG bytes (umi_icon_logo_png_data),
 *     - a GdkTexture decoded from those bytes (umi_icon_get_logo_texture),
 *     - a GtkPicture already set up to display the logo (umi_icon_image_logo).
 *
 * WHY EMBED A PNG IN CODE?
 *   - Keeps the example completely self-contained for new contributors.
 *   - Avoids file I/O and path headaches during early development.
 *   - You can later replace this stub with your real logo *without* changing
 *     the consumer code (loose coupling).
 *
 * GTK4 NOTES:
 *   - We prefer GdkTexture (GTK4-native) over legacy GdkPixbuf in this file.
 *   - We create textures from in-memory bytes using gdk_texture_new_from_bytes.
 *   - Window/taskbar icons are NOT set per-window in GTK4; use EXE resources
 *     on Windows (.rc + .ico). See the big comment under umi_icon_apply_to_window.
 *
 * HOW TO SWAP IN YOUR REAL LOGO (two options):
 *   1) **Keep in-code bytes (simple)**
 *      - Replace the `g_umicom_logo_png` array below with bytes of your PNG (e.g. 64×64).
 *      - Update the comment to note the size/source.
 *
 *   2) **Ship real assets + .rc (production)**
 *      - Put `umicom.ico` (multi-size icon) under, say, `assets/`.
 *      - Create a Windows resource script `app.rc`:
 *
 *          // app.rc (very small)
 *          #include <windows.h>
 *          IDI_APPICON ICON "assets/umicom.ico"
 *
 *      - In CMakeLists.txt, add the `.rc` to the executable *only on Windows*:
 *
 *          if (WIN32)
 *            target_sources(umicom-studio-ide PRIVATE ${CMAKE_SOURCE_DIR}/src/ui/resources/app.rc)
 *            set_source_files_properties(${CMAKE_SOURCE_DIR}/src/ui/resources/app.rc
 *                                       PROPERTIES LANGUAGE RC)
 *          endif()
 *
 *      - Now the EXE carries your branded taskbar icon. The splash window will
 *        still use the in-app image widgets for logos where needed.
 *
 * LICENSE:
 *   MIT (see project root). Keep the head comment intact for attribution.
 *
 * CREDIT:
 *   Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-15
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>      /* GTK4: GdkTexture, GtkPicture, helpers            */
#include <string.h>       /* C standard header: for size_t if needed          */
#include "icon.h"         /* Our public header for this module           */
#include <stdint.h>       /* fixed-size integer types used by many coders    */

/*-----------------------------------------------------------------------------
 * EMBEDDED PNG BYTES (1×1 transparent pixel)
 *
 * WHAT IS THIS?
 *   A minimal, valid PNG file that encodes a single transparent pixel.
 *   File size is tiny (~68 bytes). This lets us ship a working icon module
 *   without depending on file paths or external resources.
 *
 * HOW TO REPLACE:
 *   - Generate bytes from your real PNG (e.g., with a small tool/script).
 *   - Replace the array below and update the size. That's it.
 *---------------------------------------------------------------------------*/

/* NOTE: This is a stub (tiny) PNG; replace with your real image when ready.  */
/*       The pointer + length API below hides the implementation detail.       */
static const unsigned char g_umicom_logo_png[] = {
    /* Standard PNG header (8 bytes)                                         */
    0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A,
    /* IHDR chunk (13 bytes data)                                            */
    0x00,0x00,0x00,0x0D,                    /* length                        */
    0x49,0x48,0x44,0x52,                    /* tag 'IHDR'                    */
    0x00,0x00,0x00,0x01,                    /* width  = 1                    */
    0x00,0x00,0x00,0x01,                    /* height = 1                    */
    0x08,                                    /* bit depth = 8                 */
    0x06,                                    /* color type = RGBA             */
    0x00,                                    /* compression method            */
    0x00,                                    /* filter method                 */
    0x00,                                    /* interlace method              */
    0x1F,0x15,0xC4,0x89,                    /* CRC                           */
    /* IDAT chunk with a tiny zlib stream of a single transparent pixel      */
    0x00,0x00,0x00,0x0A,                    /* length                        */
    0x49,0x44,0x41,0x54,                    /* tag 'IDAT'                    */
    0x78,0x9C,                              /* zlib header                   */
    0x63,0x60,0x00,0x00,0x00,0x02,0x00,0x01,/* minimal compressed data       */
    0xE5,0x27,0xD4,0xA2,                    /* adler/CRC tail                */
    /* IEND chunk                                                             */
    0x00,0x00,0x00,0x00,                    /* length                        */
    0x49,0x45,0x4E,0x44,                    /* tag 'IEND'                    */
    0xAE,0x42,0x60,0x82                     /* CRC                           */
};

/* Store the total number of bytes for quick length queries.                   */
static const size_t g_umicom_logo_png_len = sizeof(g_umicom_logo_png);

/*-----------------------------------------------------------------------------
 * umi_icon_logo_png_data
 *
 * WHAT IT RETURNS:
 *   Pointer to the read-only static PNG bytes and writes the length to *out_len.
 *
 * WHY:
 *   Exposes raw bytes to advanced callers (hashing, alternative decoders)
 *   without binding them to filesystem paths.  The memory is process-lifetime.
 *---------------------------------------------------------------------------*/
const unsigned char* umi_icon_logo_png_data(size_t *out_len)
{
    /* Defensive: honor the API contract by writing a length.                  */
    if (out_len) {
        *out_len = g_umicom_logo_png_len;  /* number of bytes available       */
    }
    /* Return pointer to the embedded bytes.  Do not free this.                */
    return g_umicom_logo_png;
}

/*-----------------------------------------------------------------------------
 * umi_icon_get_logo_texture
 *
 * PURPOSE:
 *   Decode the embedded PNG into a GdkTexture (GTK4's paintable image type).
 *   This texture can be displayed by widgets like GtkPicture.
 *---------------------------------------------------------------------------*/
GdkTexture* umi_icon_get_logo_texture(void)
{
    size_t len = 0;
    const unsigned char *png = umi_icon_logo_png_data(&len);
    
    GBytes *bytes = g_bytes_new_static(png, len);  /* wrap bytes */
    GError *err = NULL;
    GdkTexture *tex = gdk_texture_new_from_bytes(bytes, &err);
    g_bytes_unref(bytes);

    if (err) {                                      /* if decode failed        */
        g_warning("umi_icon_get_logo_texture: failed to decode PNG: %s",
                  err->message);
        g_error_free(err);
        return NULL;
    }

    return tex;
}

/*-----------------------------------------------------------------------------
 * umi_icon_image_logo
 *
 * PURPOSE:
 *   Build a GtkPicture widget already configured to display our logo.
 *   We ask GTK to scale the picture to the requested logical size.
 *---------------------------------------------------------------------------*/
GtkWidget* umi_icon_image_logo(int size_px)
{
    /* Clamp / default size for robustness.                                    */
    int px = (size_px > 0) ? size_px : 64;

    GdkTexture *tex = umi_icon_get_logo_texture();  /* try to decode texture   */
    if (!tex) {                                     /* fallback if decode fails */
        GtkWidget *fallback = gtk_picture_new();    /* empty picture widget    */
        gtk_widget_set_size_request(fallback, px, px); /* reserve sz */
        gtk_picture_set_can_shrink(GTK_PICTURE(fallback), TRUE); /* responsive */
        gtk_picture_set_content_fit(GTK_PICTURE(fallback),
                                    GTK_CONTENT_FIT_CONTAIN);     /* keep AR   */
        return fallback;                             /* return harmless widget */
    }

    /* Create a GtkPicture from the paintable. GtkPicture takes its own ref.   */
    GtkWidget *pic = gtk_picture_new_for_paintable(GDK_PAINTABLE(tex));

    /* Tidy: drop our extra strong ref; the picture holds what it needs.       */
    g_object_unref(tex);

    gtk_widget_set_size_request(pic, px, px);                  /* size hint */
    gtk_picture_set_can_shrink(GTK_PICTURE(pic), TRUE);        /* flex      */
    gtk_picture_set_content_fit(GTK_PICTURE(pic), GTK_CONTENT_FIT_CONTAIN);

    return pic;                                      /* ready-to-pack widget    */
}

/*-----------------------------------------------------------------------------
 * umi_icon_apply_to_window
 *
 * PURPOSE (GTK4: intentionally a no-op)
 *   In GTK3 you could set a per-window icon via gtk_window_set_icon(). GTK4
 *   removed that API; app/taskbar icons on Windows come from the executable's
 *   embedded resources (.rc + .ico). We keep a stub here for API stability,
 *   teaching users the correct production path.
 *
 * HOW TO BRAND THE TASKBAR ICON ON WINDOWS (recap):
 *   - Create an .ico file with multiple sizes (16/24/32/48/64/128/256).
 *   - Add a minimal resource script `app.rc` (placed in this same folder):
 *
 *       #include <windows.h>
 *       IDI_APPICON ICON "assets/umicom.ico"
 *
 *   - Wire it in CMake (Windows only), example:
 *
 *       if (WIN32)
 *         target_sources(umicom-studio-ide PRIVATE ${CMAKE_SOURCE_DIR}/src/ui/resources/app.rc)
 *         set_source_files_properties(${CMAKE_SOURCE_DIR}/src/ui/resources/app.rc
 *                                    PROPERTIES LANGUAGE RC)
 *       endif()
 *
 *   - Rebuild. Now the EXE shows your branded icon in Explorer/taskbar.
 *---------------------------------------------------------------------------*/
void umi_icon_apply_to_window(GtkWindow *win)
{
    (void)win; /* placeholder; intentionally does nothing on GTK4 */
}

/*-----------------------------------------------------------------------------
 * umi_icon_try_apply_headerbar_logo
 *
 * PURPOSE:
 *   Augment a window's header bar by prepending a small logo image.
 *   Safe no-op if the window has no custom header bar.
 *---------------------------------------------------------------------------*/
void umi_icon_try_apply_headerbar_logo(GtkWindow *win, int desired_px)
{
    /* Defensive: ignore invalid input.                                        */
    if (!win) return;

    /* Fetch the custom titlebar (if any).  NULL => internal default.          */
    GtkWidget *titlebar = gtk_window_get_titlebar(win);
    if (!titlebar) return;

    /* Only proceed if the custom bar is a GtkHeaderBar.                       */
    if (!GTK_IS_HEADER_BAR(titlebar)) return;

    /* Create a tiny logo widget using our own helper.                          */
    int px = (desired_px > 0) ? desired_px : 16;
    GtkWidget *pic = umi_icon_image_logo(px);
    if (!pic) return;

    /* GTK4 path:
     * There are no pack_start/pack_end helpers anymore. The supported idiom
     * is to own the *title widget*. We preserve any existing title widget by
     * wrapping it, then set that composite back onto the header bar.          */
    GtkHeaderBar *hb = GTK_HEADER_BAR(titlebar);

    /* Grab any existing title widget (may be NULL).                           */
    GtkWidget *old_title = gtk_header_bar_get_title_widget(hb);

    /* Build a small row: [ logo ][ gap ][ old_title (if any) ]                */
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6 /*px gap*/);
    gtk_widget_set_hexpand(row, TRUE); /* behave like a normal title widget   */

    /* Prepend our logo.                                                       */
    gtk_box_append(GTK_BOX(row), pic);

    if (old_title) {
        /* Reparent safely:
         * 1) Take a ref so it survives unparenting,
         * 2) Clear from header bar,
         * 3) Append into our row,
         * 4) Drop the extra ref (row holds it now).                           */
        g_object_ref(old_title);
        gtk_header_bar_set_title_widget(hb, NULL);
        gtk_box_append(GTK_BOX(row), old_title);
        g_object_unref(old_title);
    }

    /* Install the composed row as the new title widget.                       */
    gtk_header_bar_set_title_widget(hb, row);
}

/*-----------------------------------------------------------------------------
 * umi_icon_try_apply_taskbar_icon_win32
 *
 * PURPOSE:
 *   Future Win32 taskbar icon hook (safe no-op today).
 *   Keep this symbol so callers can link even on non-Windows builds.  A real
 *   implementation would use Win32 APIs (or the .rc resource approach).
 *---------------------------------------------------------------------------*/
void umi_icon_try_apply_taskbar_icon_win32(GtkWindow *win)
{
    (void)win; /* placeholder; intentionally does nothing today */
}

/*---------------------------------------------------------------------------*/
/*  END OF FILE *//* END OF FILE (icon.c)                                    */
 /*---------------------------------------------------------------------------
 * APPENDIX: Notes for new contributors (kept as comments for teaching)
 *
 * Q: Why not GResource / .gresource.xml here?
 *    - You *can* ship icons via GResource (GTK’s resource system). However,
 *      you asked to prefer pure C over XML, so we demonstrate the “embed
 *      bytes in C and decode at runtime” approach. Both are valid options.
 *
 * Q: Will a 1×1 PNG look blurry?
 *    - Of course — it’s just a placeholder. Swap in a real logo PNG (e.g.
 *      256×256) by replacing kLogoPng[]. All code above will continue to work.
 *
 * Q: Any performance costs?
 *    - Decoding a single small PNG once at startup is negligible. If you add
 *      many icons, consider lazy-loading or caching textures as needed.
 *
 * Q: Is this module safe to include everywhere?
 *    - Yes. It only depends on GTK headers and exposes a tiny, stable API.
 *      No global state besides the read-only bytes is kept.
 *---------------------------------------------------------------------------*/


