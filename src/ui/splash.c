/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ui/splash.c
 *
 * PURPOSE:
 *   A tiny, pure-C splash screen that displays your logo and a couple of
 *   strings while the rest of the app initializes. No GtkBuilder, no XML.
 *
 * DESIGN:
 *   - We try to load `res/umicom-256.png` (or .svg) from disk for sharpness.
 *   - If the file is absent, we fall back to a tiny built-in XPM logo.
 *   - The splash is borderless, centered, and auto-closes after a delay.
 *
 * USAGE:
 *   GtkWidget *s = uside_splash_show(app, 600);
 *   // do initialization...
 *   uside_splash_close_later(s, 200);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>        /* GTK4 widgets                                 */
#include "icons.h"          /* uside_logo_texture_from_disk_or_fallback()    */

/* Helper: create a label with subtle styling for the splash caption.         */
static GtkWidget *mk_label(const char *txt, float xalign, int top_margin)
{
    GtkWidget *l = gtk_label_new(txt);                       /* text widget  */
    gtk_label_set_xalign(GTK_LABEL(l), xalign);              /* alignment    */
    gtk_widget_set_margin_top(l, top_margin);                /* spacing      */
    gtk_widget_set_margin_start(l, 20);
    gtk_widget_set_margin_end(l,   20);
    return l;                                                /* hand it back */
}

/* Public: show the splash window. Returns the window so callers can close it.*/
GtkWidget *uside_splash_show(GtkApplication *app, guint display_ms)
{
    if (!app) return NULL;                                   /* guard null   */

    /* Create a plain top-level window bound to the application.              */
    GtkWidget *win = gtk_application_window_new(app);

    /* Make it look like a splash: small, centered, non-resizable.            */
    gtk_window_set_title(GTK_WINDOW(win), "Umicom Studio IDE");
    gtk_window_set_decorated(GTK_WINDOW(win), FALSE);        /* no borders   */
    gtk_window_set_default_size(GTK_WINDOW(win), 480, 320);  /* size hint    */
    gtk_window_set_resizable(GTK_WINDOW(win), FALSE);        /* fixed size   */
    gtk_window_present(GTK_WINDOW(win));                     /* map to screen*/

    /* Content container: vertical box for image + labels.                     */
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_window_set_child(GTK_WINDOW(win), box);

    /* Big logo (try disk PNG/SVG; fall back to built-in XPM).                 */
    {
        GdkTexture *tex = uside_logo_texture_from_disk_or_fallback(256);
        GtkWidget  *img = gtk_image_new_from_paintable(GDK_PAINTABLE(tex));
        g_object_unref(tex);                                 /* drop our ref */
        gtk_widget_set_margin_top(img, 20);
        gtk_box_append(GTK_BOX(box), img);
    }

    /* Project title under the logo.                                          */
    gtk_box_append(GTK_BOX(box), mk_label("Umicom Studio IDE", 0.5f, 8));
    /* Small subtitle with credits/license.                                    */
    gtk_box_append(GTK_BOX(box), mk_label("© Umicom Foundation — MIT License", 0.5f, 2));

    /* Optional auto-close timer, useful when startup finishes fast.           */
    if (display_ms > 0) {
        g_timeout_add_once(display_ms, (GSourceOnceFunc)gtk_window_close, win);
    }

    return win;                                              /* to caller    */
}

/* Public: close the splash after a short grace period (0 = now).             */
void uside_splash_close_later(GtkWidget *splash_win, guint grace_ms)
{
    if (!splash_win) return;                                 /* safe on NULL */
    if (grace_ms == 0) {                                     /* immediate    */
        gtk_window_close(GTK_WINDOW(splash_win));
        return;
    }
    g_timeout_add_once(grace_ms, (GSourceOnceFunc)gtk_window_close, splash_win);
}
/*---------------------------------------------------------------------------*/
+/*  END OF FILE */