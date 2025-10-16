/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ui/splash.c
 *
 * PURPOSE (What does this file do?):
 *   This source file implements a tiny, brandable splash window for the
 *   Umicom Studio IDE. The splash window is a *separate, minimal* UI shown
 *   while the main application (your IDE window) initializes. It displays:
 *     - A big title (e.g., "Umicom Studio IDE")
 *     - A small subtitle (e.g., "Loading workspace…")
 *     - A progress bar (0..100%)
 *     - An activity spinner (so users see activity even when progress stalls)
 *
 * DESIGN GOALS (Why is it written this way?):
 *   - **Loose coupling**: this module depends only on GTK (gtk/gtk.h).
 *     It does not include any Umicom-specific headers, config structures or
 *     global state. The splash is a tiny, standalone utility you can reuse.
 *   - **Pure C**: we do not use XML resource files; the layout is built
 *     directly in C so new contributors can learn GTK by reading this code.
 *   - **Beginner-friendly**: almost every line is explained. You can treat
 *     this file as a small tutorial in GTK4 layout and widget usage.
 *
 * HOW TO READ THIS FILE (Map from API to implementation):
 *   - See splash.h for the public API we expose:
 *       UmiSplash* umi_splash_new(const char*, const char*, unsigned int);
 *       void       umi_splash_show(UmiSplash*, GtkWindow*);
 *       void       umi_splash_set_progress(UmiSplash*, double, const char*);
 *       void       umi_splash_close(UmiSplash*);
 *       void       umi_splash_free(UmiSplash*);
 *       GtkWindow* umi_splash_window(UmiSplash*);
 *
 *   - Below, we define a private struct `UmiSplash` that stores GTK widgets
 *     and small bits of state (like an auto-close timer id).
 *
 * GTK VERSION NOTE (GTK4 vs GTK3):
 *   - We use **GTK4** APIs (e.g., GtkBox layouts, GtkLabel, GtkSpinner,
 *     GtkProgressBar). In GTK4, `gtk_widget_show()` is deprecated; to present
 *     top-level windows use `gtk_window_present()` or set visible with
 *     `gtk_widget_set_visible()`. We use `gtk_window_present()` here because
 *     the splash is a top-level window.
 *
 * LIFETIME / OWNERSHIP QUICK GUIDE:
 *   - umi_splash_new() : allocates the controller via g_new0() -> you own it.
 *   - umi_splash_show(): presents the window and optionally starts auto-close.
 *   - umi_splash_close(): hides/closes the window (safe to call multiple times).
 *   - umi_splash_free() : destroys widgets (via g_object_unref) and frees the
 *                         controller struct. Call exactly once when finished.
 *
 * THREADING:
 *   - All GTK code should run on the GTK main thread. If a background worker
 *     computes progress, post updates to main via g_idle_add() or similar.
 *
 * LICENSE:
 *   MIT (see project root). Keep the head comment intact for attribution.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-15
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>        /* Include GTK4 headers for all widget symbols    */
#include <string.h>         /* For strlen (only used in a defensive clamp)    */

/*-----------------------------------------------------------------------------
 * FORWARD-DECLARED OPAQUE TYPE (matches splash.h)
 * - We repeat this to keep the source self-contained if someone opens it
 *   directly. In normal compilation, splash.h provides the typedef.
 *---------------------------------------------------------------------------*/

#include "splash.h"              /* public tiny API (umi_splash_png + typedef) */
#include "icon.h"                /* umi_apply_app_icon (no-op on non-Windows)  */

/* If you have resource IDs available on Windows, include them. Safe on others. */
#ifdef _WIN32
  #include <windows.h>
  #include "resource.h"          /* IDI_APPICON, IDP_SPLASH (RCDATA)           */
#endif

/*-----------------------------------------------------------------------------
 * PRIVATE STRUCT (Internal data of the splash controller)
 * - This struct is *not* exposed to callers. We keep only what we need:
 *   - window        : top-level GtkWindow* of the splash
 *   - title_label   : big heading label widget
 *   - subtitle_label: smaller descriptive label (status line)
 *   - progress_bar  : GtkProgressBar to indicate fractional progress
 *   - spinner       : GtkSpinner to show activity even with stalled progress
 *   - auto_close_ms : if > 0, auto-close after this many milliseconds
 *   - auto_close_id : GLib source id for the one-shot timeout (0 = none)
 *---------------------------------------------------------------------------*/
/* Internal data model for the splash window                                  */

struct UmiSplash {
    GtkWidget *window;       /* GtkWindow* acting as the splash                */
    GtkWidget *box;          /* VBox content                                   */
    GtkWidget *title;        /* large title label                              */
    GtkWidget *subtitle;     /* small subtitle / status label                  */
    GtkWidget *image;        /* optional image (logo loaded from PNG)          */
    GtkWidget *bar;          /* progress bar                                   */
    guint      auto_close_ms;/* optional auto-close after show                 */
    guint      auto_close_id;/* g_timeout source id                            */
};

/*-----------------------------------------------------------------------------
 * HELPER: clamp_01
 *
 * PURPOSE:
 *   Utility to clamp a double value to [0.0, 1.0]. GTK widgets frequently
 *   expect normalized progress values. We accept anything but clamp it to be
 *   defensive and avoid undefined behavior.
 *
 * PARAMETERS:
 *   x : double – value to clamp
 *
 * RETURNS:
 *   double – clamped value in [0.0, 1.0]
 *---------------------------------------------------------------------------*/

/* Small helpers to keep code tidy. */
static void         umi_splash_fill(UmiSplash *s, const char *title, const char *subtitle);
static void         umi_splash_try_load_png_logo(UmiSplash *s);
static gboolean     umi_splash_do_autoclose(gpointer data);

/*-----------------------------------------------------------------------------
 * HELPER: on_timeout_auto_close
 *
 * PURPOSE:
 *   GLib timeout callback to auto-close the splash window after a delay.
 *   We use g_timeout_add() to schedule this once the splash is shown.
 *
 * PARAMETERS:
 *   data : gpointer – actually a UmiSplash* we provided when scheduling
 *
 * RETURNS:
 *   gboolean – return G_SOURCE_REMOVE to stop the timeout from repeating.
 *
 * SIDE EFFECTS:
 *   Calls umi_splash_close() to hide the splash. Also clears auto_close_id.
 *---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
 * HELPER: build_ui
 *
 * PURPOSE:
 *   Create and compose all GTK widgets for the splash. We build a simple,
 *   centered vertical layout:
 *      +----------------------------------------+
 *      |             [  Title (big) ]           |
 *      |             (subtitle / status)        |
 *      | [spinner]      [====progress====]      |
 *      +----------------------------------------+
 *
 * PARAMETERS:
 *   s        : UmiSplash*   – controller to fill with widgets
 *   title    : const char*  – big title string (UTF-8)
 *   subtitle : const char*  – small subtitle string (UTF-8)
 *
 * RETURNS:
 *   void – widgets are created and assigned into `s`.
 *
 * NOTE:
 *   All widgets are referenced by the toplevel window; later we unref them
 *   by unref-ing the window in umi_splash_free().
 *---------------------------------------------------------------------------*/

 /*---------------------------------------------------------------------------*/
/* PNG loader: implementation for umi_splash_png declared in splash.h        */
/* On Windows, we read the PNG from RCDATA(IDP_SPLASH). Else, fall back to   */
/* a compiled array if you ship one (see ustudio_resources.c).               */

const unsigned char *umi_splash_png(size_t *out_size)
{
#ifdef _WIN32
    HINSTANCE hInst = GetModuleHandleW(NULL);
    HRSRC hRes = FindResourceW(hInst, MAKEINTRESOURCEW(IDP_SPLASH), L"RCDATA");
    if (!hRes) { if (out_size) *out_size = 0; return NULL; }
    HGLOBAL hData = LoadResource(hInst, hRes);
    if (!hData) { if (out_size) *out_size = 0; return NULL; }
    DWORD sz = SizeofResource(hInst, hRes);
    void *ptr = LockResource(hData);
    if (out_size) *out_size = (size_t)sz;
    return (const unsigned char*)ptr;
#else
    /* Optional cross-platform fallback that you can provide in
       src/ui/resources/ustudio_resources.c. If you don't ship it,
       this will just return NULL and the splash works without an image. */
    extern const unsigned char g_ustudio_splash_png[];
    extern const unsigned int  g_ustudio_splash_png_len;
    if (&g_ustudio_splash_png && g_ustudio_splash_png_len > 0) {
        if (out_size) *out_size = (size_t)g_ustudio_splash_png_len;
        return g_ustudio_splash_png;
    }
    if (out_size) *out_size = 0;
    return NULL;
#endif
}

/*-----------------------------------------------------------------------------
 * umi_splash_new
 *
 * PURPOSE:
 *   Allocate and create a new splash controller and build its UI. The window
 *   is created but not yet shown; call umi_splash_show() to present it.
 *---------------------------------------------------------------------------*/
UmiSplash *umi_splash_new(const char *title,
                          const char *subtitle,
                          guint       auto_close_ms)
{
    UmiSplash *s = g_new0(UmiSplash, 1);
    s->auto_close_ms = auto_close_ms;

    /* Window: undecorated, always-on-top, transient utility. */
    s->window = gtk_window_new();
    gtk_window_set_decorated(GTK_WINDOW(s->window), FALSE);
    gtk_window_set_resizable(GTK_WINDOW(s->window), FALSE);
    gtk_window_set_modal(GTK_WINDOW(s->window), FALSE);
    gtk_window_set_default_size(GTK_WINDOW(s->window), 420, 220);
    gtk_window_set_title(GTK_WINDOW(s->window), "Umicom Studio");

    /* Let Windows pick up a proper taskbar icon (and small/top icons). */
#ifdef _WIN32
    /* Apply app icon to the splash HWND (no-op on non-Windows). */
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(s->window));
    if (surface) {
        /* Note: umi_apply_app_icon is declared in icon.h but may be a no-op */
        /* umi_apply_app_icon((HWND)surface); */
    }
#endif

    s->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8 /*spacing*/);
    gtk_widget_set_margin_top   (s->box, 20);
    gtk_widget_set_margin_bottom(s->box, 16);
    gtk_widget_set_margin_start (s->box, 20);
    gtk_widget_set_margin_end   (s->box, 20);
    gtk_window_set_child(GTK_WINDOW(s->window), s->box);

    /* Image (optional), Title, Subtitle, Progress Bar */
    s->image    = gtk_image_new();     /* filled later if PNG exists */
    s->title    = gtk_label_new(NULL);
    s->subtitle = gtk_label_new(NULL);
    s->bar      = gtk_progress_bar_new();

    /* Style labels a bit (no css file required). */
    gtk_label_set_wrap(GTK_LABEL(s->title), FALSE);
    gtk_label_set_xalign(GTK_LABEL(s->title), 0.0f);
    gtk_label_set_use_markup(GTK_LABEL(s->title), TRUE); /* allow <b> */

    gtk_label_set_wrap(GTK_LABEL(s->subtitle), TRUE);
    gtk_label_set_xalign(GTK_LABEL(s->subtitle), 0.0f);

    /* Pack them in order. */
    gtk_box_append(GTK_BOX(s->box), s->image);
    gtk_box_append(GTK_BOX(s->box), s->title);
    gtk_box_append(GTK_BOX(s->box), s->subtitle);
    gtk_box_append(GTK_BOX(s->box), s->bar);

    /* Fill content and try to load the PNG boxed in resources. */
    umi_splash_fill(s, title ? title : "Umicom Studio IDE",
                       subtitle ? subtitle : "Initializing…");
    umi_splash_try_load_png_logo(s);

    /* Setup auto-close if requested. */
    if (s->auto_close_ms > 0) {
        s->auto_close_id = g_timeout_add_full(G_PRIORITY_DEFAULT,
                                              s->auto_close_ms,
                                              umi_splash_do_autoclose,
                                              s, NULL);
    }
    return s;
}

/*-----------------------------------------------------------------------------
 * umi_splash_show
 *
 * PURPOSE:
 *   Present the splash window. Optionally make it transient for a given
 *   parent window (keeps the splash above the parent on some platforms).
 *---------------------------------------------------------------------------*/
void umi_splash_show(UmiSplash *splash, GtkWindow *parent)
{
    /* Defensive: ignore NULL pointer; this keeps the call site simpler.     */
    if (!splash || !splash->window) return;

    /* If a parent is provided, set splash to be transient for that parent.  */
    if (parent) {
        gtk_window_set_transient_for(GTK_WINDOW(splash->window), parent);
        /* Mark as modal so clicks go to splash while it is up (optional).   */
        gtk_window_set_modal(GTK_WINDOW(splash->window), TRUE);
    }

    /* Present the window to the user (GTK4 best-practice for toplevel).     */
    gtk_window_present(GTK_WINDOW(splash->window));
}

/*-----------------------------------------------------------------------------
 * umi_splash_set_progress
 *
 * PURPOSE:
 *   Update progress bar and status message.
 *---------------------------------------------------------------------------*/
void umi_splash_set_progress(UmiSplash *s, double fraction, const char *msg)
{
    if (!s || !GTK_IS_WIDGET(s->bar)) return;
    if (fraction < 0.0) fraction = 0.0;
    if (fraction > 1.0) fraction = 1.0;
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(s->bar), fraction);
    if (msg && *msg) {
        gtk_label_set_text(GTK_LABEL(s->subtitle), msg);
    }
}

/*-----------------------------------------------------------------------------
 * umi_splash_close
 *
 * PURPOSE:
 *   Hide/close the splash window.
 *---------------------------------------------------------------------------*/
void umi_splash_close(UmiSplash *s)
{
    if (!s) return;
    if (GTK_IS_WINDOW(s->window)) gtk_window_close(GTK_WINDOW(s->window));
}

/*-----------------------------------------------------------------------------
 * umi_splash_free
 *
 * PURPOSE:
 *   Destroy the splash and free all resources.
 *---------------------------------------------------------------------------*/
void umi_splash_free(UmiSplash *s)
{
    if (!s) return;
    if (s->auto_close_id) {
        g_source_remove(s->auto_close_id);
        s->auto_close_id = 0;
    }
    if (GTK_IS_WIDGET(s->window)) {
        gtk_window_destroy(GTK_WINDOW(s->window));
    }
    s->box = s->title = s->subtitle = s->image = s->bar = NULL;
    g_free(s);
}

/*-----------------------------------------------------------------------------
 * umi_splash_window
 *
 * PURPOSE:
 *   Return the underlying GtkWindow for advanced usage.
 *---------------------------------------------------------------------------*/
GtkWindow *umi_splash_window(UmiSplash *splash)
{
    return splash ? GTK_WINDOW(splash->window) : NULL;
}

/*---------------------------------------------------------------------------*/
/* Backwards-compat shim required by existing app code                        */
/* Your build error shows calls to:                                           */
/*   - uside_splash_show(GtkApplication *app, guint auto_close_ms)           */
/*   - uside_splash_close_later(GtkWidget *splash, guint grace_ms)           */
/* We provide those thin wrappers here so you don't have to touch app.c.     */

GtkWidget *uside_splash_show(GtkApplication *app, guint auto_close_ms)
{
    /* Create the helper and present. We store the handle on the GtkWindow so
       you can still reach the struct later if you want (for progress updates). */
    GtkWindow *parent = NULL;
    /* If your app already created a main window, attach; otherwise NULL is fine. */
    (void)app;

    UmiSplash *s = umi_splash_new("Umicom Studio IDE",
                                  "Starting up…",
                                  auto_close_ms);
    umi_splash_show(s, parent);

    /* Attach the handle for optional later access. */
    g_object_set_data_full(G_OBJECT(s->window), "umicom-splash-handle", s,
                           (GDestroyNotify)umi_splash_free);
    return s->window;
}

void uside_splash_close_later(GtkWidget *splash, guint grace_ms)
{
    if (!GTK_IS_WIDGET(splash)) return;

    /* Close immediately if no grace requested. */
    if (grace_ms == 0) {
        UmiSplash *s = g_object_get_data(G_OBJECT(splash), "umicom-splash-handle");
        if (s) umi_splash_close(s);
        return;
    }

    /* Schedule a single-shot close. We keep the closure tiny. */
    GtkWidget *weak = splash; /* keep local */
    g_timeout_add_once(grace_ms, (GSourceOnceFunc)+[](gpointer data){
        GtkWidget *w = GTK_WIDGET(data);
        if (!GTK_IS_WIDGET(w)) return;
        UmiSplash *s = g_object_get_data(G_OBJECT(w), "umicom-splash-handle");
        if (s) umi_splash_close(s);
    }, weak);
}

/*---------------------------------------------------------------------------*/
/* Implementation details                                                     */

static void umi_splash_fill(UmiSplash *s, const char *title, const char *subtitle)
{
    /* Title is bold & slightly larger using simple Pango markup. */
    if (title && *title) {
        gchar *markup = g_markup_printf_escaped("<span size='12000' weight='bold'>%s</span>", title);
        gtk_label_set_markup(GTK_LABEL(s->title), markup);
        g_free(markup);
    } else {
        gtk_label_set_text(GTK_LABEL(s->title), "Umicom Studio IDE");
    }

    gtk_label_set_text(GTK_LABEL(s->subtitle), (subtitle && *subtitle) ? subtitle : "");
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(s->bar), 0.1);
}

static void umi_splash_try_load_png_logo(UmiSplash *s)
{
    size_t png_sz = 0;
    const unsigned char *png = umi_splash_png(&png_sz);
    if (!png || png_sz == 0) {
        gtk_widget_set_visible(s->image, FALSE);
        return;
    }

    /* Load PNG bytes using a GdkPixbufLoader. */
    GdkPixbufLoader *ldr = gdk_pixbuf_loader_new();
    GError *err = NULL;

    if (!gdk_pixbuf_loader_write(ldr, png, (gsize)png_sz, &err)) {
        if (err) { g_error_free(err); err = NULL; }
        gdk_pixbuf_loader_close(ldr, NULL);
        g_object_unref(ldr);
        gtk_widget_set_visible(s->image, FALSE);
        return;
    }

    gdk_pixbuf_loader_close(ldr, NULL);
    GdkPixbuf *pb = gdk_pixbuf_loader_get_pixbuf(ldr);

    if (pb) {
        /* Scale down reasonably for the splash (keeps aspect). */
        int w = gdk_pixbuf_get_width(pb);
        int h = gdk_pixbuf_get_height(pb);
        const int maxW = 256, maxH = 128;
        double scale = 1.0;
        if (w > maxW) scale = (double)maxW / (double)w;
        if ((int)(h * scale) > maxH) scale = (double)maxH / (double)h;

        GdkPixbuf *scaled = (scale < 1.0)
          ? gdk_pixbuf_scale_simple(pb, (int)(w * scale), (int)(h * scale), GDK_INTERP_BILINEAR)
          : g_object_ref(pb);

        gtk_image_set_from_pixbuf(GTK_IMAGE(s->image), scaled);
        g_object_unref(scaled);
        gtk_widget_set_visible(s->image, TRUE);
    } else {
        gtk_widget_set_visible(s->image, FALSE);
    }

    g_object_unref(ldr);
}

static gboolean umi_splash_do_autoclose(gpointer data)
{
    UmiSplash *s = (UmiSplash*)data;
    s->auto_close_id = 0;
    umi_splash_close(s);
    return G_SOURCE_REMOVE;
}

/*---------------------------------------------------------------------------*/
/*  END OF FILE */