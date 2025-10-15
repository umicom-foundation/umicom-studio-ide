/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ui/include/icons.c
 *
 * PURPOSE:
 *   Load the Umicom logo from disk when available, otherwise fall back to a
 *   tiny embedded XPM image. This keeps the project “pure-C” with no XML
 *   resources while still allowing easy branding.
 *
 *   We expose two APIs:
 *     - uside_logo_texture_from_disk_or_fallback(px): returns GdkTexture*
 *     - uside_create_logo_image(px): returns GtkImage* for header bars, etc.
 *
 * DISK LOOKUP STRATEGY:
 *   We probe a couple of friendly locations under `res/` (alongside the exe
 *   or the source tree) so artists can just drop new PNG/SVG files there.
 *
 * FALLBACK:
 *   A tiny 16x16 XPM array (ASCII art) that compiles on any platform.
 *   It’s intentionally simple; replace with your own XPM if you like.
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>           /* GdkPixbuf, GdkTexture, GtkImage             */
#include <string.h>            /* strcmp, strlen                              */

/* --- A tiny 16x16 XPM fallback (blue-ish box with white 'U') --------------- */
/* XPM is a C string format GTK can load without external files.               */
static const char *k_umicom_xpm_16[] = {
"16 16 3 1",
"   c None",
".  c #1B3550",
"+  c #FFFFFF",
"................",
"................",
".....++++++.....",
".....+....+.....",
".....+....+.....",
".....+....+.....",
".....+....+.....",
".....+....+.....",
".....+....+.....",
".....+....+.....",
".....+....+.....",
".....+....+.....",
".....+....+.....",
".....++++++.....",
"................",
"................"
};

/* Helper: try to load a Pixbuf from one path; returns NULL on failure.        */
static GdkPixbuf *try_load_pixbuf(const char *path)
{
    if (!path || !*path) return NULL;
    GError *err = NULL;
    GdkPixbuf *pb = gdk_pixbuf_new_from_file(path, &err);
    if (!pb && err) { g_debug("icon load failed: %s (%s)", path, err->message); g_clear_error(&err); }
    return pb;  /* may be NULL if load failed */
}

/* Helper: look for PNG/SVG in a few places. Adjust/add paths as you like.    */
static GdkPixbuf *try_load_from_common_paths(int prefer_px)
{
    (void)prefer_px; /* not strictly needed but kept for future heuristics */
    const char *candidates[] = {
        "res/umicom-256.png",
        "res/umicom-128.png",
        "res/umicom-64.png",
        "res/umicom.svg",         /* GTK can load SVG via librsvg if present */
        NULL
    };
    for (int i = 0; candidates[i]; ++i) {
        GdkPixbuf *pb = try_load_pixbuf(candidates[i]);
        if (pb) return pb;
    }
    return NULL;
}

/* Public: return a texture from disk or fallback XPM. Caller unrefs it.       */
GdkTexture *uside_logo_texture_from_disk_or_fallback(int prefer_px)
{
    /* First, try disk for a crisp PNG/SVG.                                   */
    GdkPixbuf *pb = try_load_from_common_paths(prefer_px);

    /* If that failed, manufacture a tiny Pixbuf from the embedded XPM.       */
    if (!pb) {
        pb = gdk_pixbuf_new_from_xpm_data(k_umicom_xpm_16);
    }

    /* In GTK4, drawables are Textures. Convert Pixbuf -> Texture.            */
    GdkTexture *tex = gdk_texture_new_for_pixbuf(pb);
    g_object_unref(pb);      /* drop our pixbuf reference (texture owns data) */
    return tex;              /* strong ref returned                           */
}

/* Public: convenience wrapper that returns a GtkImage widget.                 */
GtkWidget *uside_create_logo_image(int prefer_px)
{
    GdkTexture *tex = uside_logo_texture_from_disk_or_fallback(prefer_px);
    GtkWidget  *img = gtk_image_new_from_paintable(GDK_PAINTABLE(tex));
    g_object_unref(tex);  /* we hand ownership to the GtkImage */
    return img;
}
/*---------------------------------------------------------------------------*/
/*  END OF FILE */ 
