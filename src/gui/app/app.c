/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ui/splash.c
 *
 * PURPOSE (What does this file do?):
 *   This source file implements a tiny, brandable splash window for the
 *   Umicom Studio IDE. The splash window is a *separate, minimal* UI shown
 *   while the main application (your IDE window) initializes. It displays:
 *     - A big title (e.g., "Umicom Studio IDE")                                  // explain the headline role
 *     - A small subtitle (e.g., "Loading workspace…")                            // explain the status line
 *     - A progress bar (0..100%)                                                 // explain progress affordance
 *     - An activity spinner (so users see activity even when progress stalls)    // UX rationale
 *
 * DESIGN GOALS (Why is it written this way?):
 *   - **Loose coupling**: this module depends only on GTK (gtk/gtk.h).          // keep dependencies minimal
 *     It does not include any Umicom-specific headers, config structures or      // no global/project-state reach-in
 *     global state. The splash is a tiny, standalone utility you can reuse.      // reusability advantage
 *   - **Pure C**: we do not use XML resource files; the layout is built          // matches project preference
 *     directly in C so new contributors can learn GTK by reading this code.      // educational intent
 *   - **Beginner-friendly**: almost every line is explained. You can treat       // commentary density promise
 *     this file as a small tutorial in GTK4 layout and widget usage.             // learning aid
 *
 * HOW TO READ THIS FILE (Map from API to implementation):
 *   - See splash.h for the public API we expose:                                 // pointer to header
 *       UmiSplash* umi_splash_new(const char*, const char*, unsigned int);       // constructor
 *       void       umi_splash_show(UmiSplash*, GtkWindow*);                      // present window
 *       void       umi_splash_set_progress(UmiSplash*, double, const char*);     // set fraction + text
 *       void       umi_splash_close(UmiSplash*);                                  // close (non-owning)
 *       void       umi_splash_free(UmiSplash*);                                   // destroy + free
 *       GtkWindow* umi_splash_window(UmiSplash*);                                 // access GtkWindow*
 *
 *   - Below, we define a private struct `UmiSplash` that stores GTK widgets      // private state description
 *     and small bits of state (like an auto-close timer id).                     // extra internal state
 *
 * GTK VERSION NOTE (GTK4 vs GTK3):
 *   - We use **GTK4** APIs (e.g., GtkBox layouts, GtkLabel, GtkSpinner,          // API generation note
 *     GtkProgressBar). In GTK4, `gtk_widget_show()` is deprecated; to present    // deprecation guidance
 *     top-level windows use `gtk_window_present()` or set visible with           // alternative methods
 *     `gtk_widget_set_visible()`. We use `gtk_window_present()` here because     // chosen approach
 *     the splash is a top-level window.                                          // reason for choice
 *
 * LIFETIME / OWNERSHIP QUICK GUIDE:
 *   - umi_splash_new() : allocates the controller via g_new0() -> you own it.    // caller ownership
 *   - umi_splash_show(): presents the window and optionally starts auto-close.    // when to show
 *   - umi_splash_close(): hides/closes the window (safe to call multiple times). // idempotence
 *   - umi_splash_free() : destroys widgets (via g_object_unref) and frees the    // destruction path
 *                         controller struct. Call exactly once when finished.    // exactly-once requirement
 *
 * THREADING:
 *   - All GTK code should run on the GTK main thread. If a background worker     // thread safety rule
 *     computes progress, post updates to main via g_idle_add() or similar.       // safe cross-thread update
 *
 * LICENSE:
 *   MIT (see project root). Keep the head comment intact for attribution.        // licensing reminder
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-15     // authorship + date
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>        /* GTK4 public API                                  */  // core UI toolkit include
#include <string.h>         /* Defensive helpers (rarely used)                  */  // standard C helpers for completeness

/*-----------------------------------------------------------------------------
 * FORWARD-DECLARED OPAQUE TYPE (matches splash.h)
 * - We repeat this to keep the source self-contained if someone opens it
 *   directly. In normal compilation, splash.h provides the typedef.
 *---------------------------------------------------------------------------*/  // rationale for duplication for readability

#include "splash.h"         /* Public tiny API (umi_splash_png + typedefs)      */  // module’s own public interface
#include "icon.h"           /* umi_icon_apply_to_window (best-effort Windows)   */  // icon helper used under _WIN32

#ifdef _WIN32
  #include <windows.h>      /* Win32 handles for resource loading               */  // needed for FindResource/LoadResource
  #include "resource.h"     /* IDI_APPICON, IDP_SPLASH (RCDATA)                 */  // centralised resource IDs
#endif
#include "theme.h"  // header lives at src/core/include/theme.h
/*-----------------------------------------------------------------------------
 * PRIVATE STRUCT
 * - This struct is *not* exposed to callers. We keep only what we need:
 *   - window        : top-level GtkWindow* of the splash
 *   - box           : vertical box container
 *   - title         : big heading label widget
 *   - subtitle      : smaller descriptive label (status line)
 *   - image         : optional image (logo loaded from PNG)
 *   - bar           : progress bar
 *   - auto_close_ms : if > 0, auto-close after this many milliseconds
 *   - auto_close_id : GLib source id for the one-shot timeout (0 = none)
 *---------------------------------------------------------------------------*/  // high-level field overview
struct UmiSplash {
    GtkWidget *window;       /* Top-level splash window                          */  // owns all children
    GtkWidget *box;          /* VBox content container                           */  // layout root
    GtkWidget *title;        /* Large title label                                */  // headline widget
    GtkWidget *subtitle;     /* Small status line                                */  // subtext/status
    GtkWidget *image;        /* Optional logo image                              */  // branding picture
    GtkWidget *bar;          /* Progress bar                                     */  // progress indicator
    guint      auto_close_ms;/* Auto-close delay; 0 means disabled               */  // config value
    guint      auto_close_id;/* GLib timeout id; 0 means none                    */  // active timer token
};

/*-----------------------------------------------------------------------------
 * FORWARD DECLARATIONS (helpers)
 *---------------------------------------------------------------------------*/  // keep prototypes near top for readability
static void     umi_splash_fill(UmiSplash *s, const char *title, const char *subtitle);  // set labels + initial state
static void     umi_splash_try_load_png_logo(UmiSplash *s);                               // attempt brand PNG load
static gboolean umi_splash_do_autoclose(gpointer data);                                   // timeout → close handler
static gboolean umi_splash_close_timeout_cb(gpointer data);                               // typed timeout adapter

/*-----------------------------------------------------------------------------
 * PNG loader: implementation for umi_splash_png declared in splash.h
 * On Windows, we read the PNG from RCDATA(IDP_SPLASH). Else, fall back to
 * a compiled array if you ship one (see ustudio_resources.c).
 *---------------------------------------------------------------------------*/  // cross-platform resource strategy
const unsigned char *umi_splash_png(size_t *out_size)                                     // expose PNG bytes provider
{
#ifdef _WIN32
    /* Windows path: read PNG bytes from the RCDATA(IDP_SPLASH) resource.       */  // Win32 resource-based load
    HINSTANCE hInst = GetModuleHandleW(NULL);                                       // module handle for resources
    HRSRC     hRes  = FindResourceW(hInst, MAKEINTRESOURCEW(IDP_SPLASH), L"RCDATA");// locate RCDATA chunk
    if (!hRes) { if (out_size) *out_size = 0; return NULL; }                        // bail if not found
    HGLOBAL   hData = LoadResource(hInst, hRes);                                     // load the resource into memory
    if (!hData) { if (out_size) *out_size = 0; return NULL; }                        // bail if load failed
    DWORD     sz    = SizeofResource(hInst, hRes);                                   // byte length of data
    void     *ptr   = LockResource(hData);                                           // stable pointer to bytes
    if (out_size) *out_size = (size_t)sz;                                            // report size to caller
    return (const unsigned char*)ptr;                                                // return raw PNG bytes
#else
    /* Cross-platform fallback:
     * If you ship compiled-in PNG bytes (see src/ui/resources/ustudio_resources.c),
     * expose them as g_ustudio_splash_png / len. Otherwise, returning NULL is fine;
     * the splash will just hide the image and show text only.
     */                                                                              // explain fallback plan
    extern const unsigned char g_ustudio_splash_png[];                                // compiled-in PNG array
    extern const unsigned int  g_ustudio_splash_png_len;                              // length of array
    if (&g_ustudio_splash_png && g_ustudio_splash_png_len > 0) {                      // validate presence
        if (out_size) *out_size = (size_t)g_ustudio_splash_png_len;                   // set size outparam
        return g_ustudio_splash_png;                                                  // return data pointer
    }
    if (out_size) *out_size = 0;                                                      // signal absence
    return NULL;                                                                       // no image available
#endif
}

/*-----------------------------------------------------------------------------
 * umi_splash_new — build the splash widgets (not shown yet)
 *
 * PURPOSE:
 *   Allocate and create a new splash controller and build its UI. The window
 *   is created but not yet shown; call umi_splash_show() to present it.
 *---------------------------------------------------------------------------*/  // constructor docs
UmiSplash *umi_splash_new(const char *title,
                          const char *subtitle,
                          guint       auto_close_ms)
{
    UmiSplash *s = g_new0(UmiSplash, 1);                                             // allocate zeroed controller
    s->auto_close_ms = auto_close_ms;                                                // remember auto-close config

    /* Window: undecorated utility top-level */                                      // top-level window setup
    s->window = gtk_window_new();                                                    // create GTK4 window
    gtk_window_set_decorated(GTK_WINDOW(s->window), FALSE);                          // hide system decorations
    gtk_window_set_resizable(GTK_WINDOW(s->window), FALSE);                          // fixed splash size
    gtk_window_set_modal(GTK_WINDOW(s->window), FALSE);                              // not modal by default
    gtk_window_set_default_size(GTK_WINDOW(s->window), 420, 220);                    // sensible default size
    gtk_window_set_title(GTK_WINDOW(s->window), "Umicom Studio");                    // window title text

#ifdef _WIN32
    /* Optional: apply taskbar and titlebar icon on Windows (best-effort).
     * We now call the *declared* helper from icon.h so the symbol exists.
     * If the helper is a no-op, this is still harmless and future-proof.
     */                                                                              // fix: use correct helper
    umi_icon_apply_to_window(GTK_WINDOW(s->window)); /* best-effort app icon on Win32 */
#endif

    /* Layout: vertical box with margins */                                          // build main container
    s->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);                               // vbox with spacing
    gtk_widget_set_margin_top   (s->box, 20);                                        // top margin for breathing room
    gtk_widget_set_margin_bottom(s->box, 16);                                        // bottom margin
    gtk_widget_set_margin_start (s->box, 20);                                        // left margin
    gtk_widget_set_margin_end   (s->box, 20);                                        // right margin
    gtk_window_set_child(GTK_WINDOW(s->window), s->box);                              // attach to window

    /* Child widgets */                                                              // create contents
    s->image    = gtk_image_new();                                                   // image placeholder
    s->title    = gtk_label_new(NULL);                                               // title label (markup later)
    s->subtitle = gtk_label_new(NULL);                                               // subtitle label
    s->bar      = gtk_progress_bar_new();                                            // progress indicator

    /* Label styling (minimal, self-contained) */                                    // basic label setup
    gtk_label_set_wrap(GTK_LABEL(s->title), FALSE);                                  // no wrap for title
    gtk_label_set_xalign(GTK_LABEL(s->title), 0.0f);                                 // left align title
    gtk_label_set_use_markup(GTK_LABEL(s->title), TRUE);                             // enable Pango markup

    gtk_label_set_wrap(GTK_LABEL(s->subtitle), TRUE);                                // allow subtitle wrap
    gtk_label_set_xalign(GTK_LABEL(s->subtitle), 0.0f);                              // left align subtitle

    /* Pack: image → title → subtitle → progress bar */                              // packing order
    gtk_box_append(GTK_BOX(s->box), s->image);                                       // add image to vbox
    gtk_box_append(GTK_BOX(s->box), s->title);                                       // add title to vbox
    gtk_box_append(GTK_BOX(s->box), s->subtitle);                                    // add subtitle to vbox
    gtk_box_append(GTK_BOX(s->box), s->bar);                                         // add progress to vbox

    /* Fill text and try to show a logo if we have PNG bytes */                      // initial content fill
    umi_splash_fill(s, title ? title : "Umicom Studio IDE",                           // set title with default
                       subtitle ? subtitle : "Initializing…");                        // set subtitle with default
    umi_splash_try_load_png_logo(s);                                                  // attempt logo load

    /* Optional auto-close timer (safe lifetime; cleared in free) */                 // schedule auto-close if requested
    if (s->auto_close_ms > 0) {                                                       // only if enabled
        s->auto_close_id = g_timeout_add_full(G_PRIORITY_DEFAULT,                     // default priority
                                              s->auto_close_ms,                       // delay in ms
                                              umi_splash_do_autoclose,                // callback (below)
                                              s,                                      // user data (controller)
                                              NULL);                                  // no destroy notify (controller outlives)
    }
    return s;                                                                         // hand ownership to caller
}

/*-----------------------------------------------------------------------------
 * umi_splash_show — present splash; keep above parent if provided
 *
 * PURPOSE:
 *   Present the splash window. Optionally make it transient for a given
 *   parent window (keeps the splash above the parent on some platforms).
 *---------------------------------------------------------------------------*/  // presentation docs
void umi_splash_show(UmiSplash *splash, GtkWindow *parent)
{
    if (!splash || !splash->window) return;                                          // defensive null checks

    if (parent) {                                                                     // parent provided?
        gtk_window_set_transient_for(GTK_WINDOW(splash->window), parent);             // keep-on-top of parent
        gtk_window_set_modal(GTK_WINDOW(splash->window), TRUE);                       // trap clicks to splash
    }

    gtk_window_present(GTK_WINDOW(splash->window));                                   // show top-level window
}

/*-----------------------------------------------------------------------------
 * umi_splash_set_progress — update bar and status text
 *
 * PURPOSE:
 *   Update progress bar and status message.
 *---------------------------------------------------------------------------*/  // updater docs
void umi_splash_set_progress(UmiSplash *s, double fraction, const char *msg)
{
    if (!s || !GTK_IS_WIDGET(s->bar)) return;                                        // validate pointers
    if (fraction < 0.0) fraction = 0.0;                                              // clamp low
    if (fraction > 1.0) fraction = 1.0;                                              // clamp high
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(s->bar), fraction);               // set normalized progress
    if (msg && *msg) gtk_label_set_text(GTK_LABEL(s->subtitle), msg);                // update status text if provided
}

/*-----------------------------------------------------------------------------
 * umi_splash_close — hide/close splash window
 *
 * PURPOSE:
 *   Hide/close the splash window.
 *---------------------------------------------------------------------------*/  // closing docs
void umi_splash_close(UmiSplash *s)
{
    if (!s) return;                                                                   // guard null
    if (GTK_IS_WINDOW(s->window)) {                                                   // ensure valid window
        gtk_window_close(GTK_WINDOW(s->window));                                      // request close (GTK4-safe)
    }
}

/*-----------------------------------------------------------------------------
 * umi_splash_free — cancel timer, destroy window, free controller
 *
 * PURPOSE:
 *   Destroy the splash and free all resources.
 *---------------------------------------------------------------------------*/  // destructor docs
void umi_splash_free(UmiSplash *s)
{
    if (!s) return;                                                                   // guard null

    if (s->auto_close_id) {                                                           // if a timer is active
        g_source_remove(s->auto_close_id);                                            // cancel it
        s->auto_close_id = 0;                                                         // clear token
    }

    if (GTK_IS_WIDGET(s->window)) {                                                   // if window still alive
        gtk_window_close(GTK_WINDOW(s->window));                                      // request close
        g_object_unref(s->window);                                                    // drop our ref to free it
    }

    s->window = s->box = s->title = s->subtitle = s->image = s->bar = NULL;           // null out pointers
    g_free(s);                                                                        // free controller memory
}

/*-----------------------------------------------------------------------------
 * umi_splash_window — expose GtkWindow* if a caller needs it
 *---------------------------------------------------------------------------*/  // accessor docs
GtkWindow *umi_splash_window(UmiSplash *splash)
{
    return splash ? GTK_WINDOW(splash->window) : NULL;                                // return inner window or NULL
}

/*-----------------------------------------------------------------------------
 * Small convenience shims (kept for callers that want quick one-liners)
 *   - GtkWidget *umi_splash_show_simple(GtkApplication*, guint auto_close_ms)
 *   - void        umi_splash_close_later(GtkWidget*, guint grace_ms)
 *
 * These help simple call sites without forcing them to manage the handle.
 *---------------------------------------------------------------------------*/  // convenience helpers
GtkWidget *umi_splash_show_simple(GtkApplication *app, guint auto_close_ms)
{
    (void)app;                                                                        // not used here (kept for ABI)

    UmiSplash *s = umi_splash_new("Umicom Studio IDE",                                 // default title
                                  "Starting up…",                                      // default subtitle
                                  auto_close_ms);                                      // pass-through timeout
    umi_splash_show(s, /*parent*/ NULL);                                               // present now

    /* Attach `s` to the window so cleanup happens automatically when            */   // avoid leaks in shim usage
    /* the window is finally destroyed (prevents leaks when using the shim).     */
    g_object_set_data_full(G_OBJECT(s->window), "umicom-splash-handle", s,            // attach controller to GObject
                           (GDestroyNotify)umi_splash_free);                          // auto-free when widget dies
    return s->window;                                                                 // return GtkWidget* to caller
}

/* NOTE: this is the callback used by umi_splash_close_later(). It’s now
 * safe against use-after-free because we retain a ref to the GtkWidget
 * for the duration of the timeout.
 */                                                                                   // UAF-prevention note
static gboolean umi_splash_close_timeout_cb(gpointer data)
{
    GtkWidget *splash_widget = GTK_WIDGET(data);                                      // cast user data to widget
    if (!G_IS_OBJECT(splash_widget)) return G_SOURCE_REMOVE;                          // validate object

    /* Retrieve the controller and close via the supported API.                */     // fetch controller handle
    UmiSplash *s = g_object_get_data(G_OBJECT(splash_widget), "umicom-splash-handle");// recover UmiSplash*
    if (s) umi_splash_close(s);                                                       // close via official API

    return G_SOURCE_REMOVE;                                                           // one-shot timer: remove source
}

void umi_splash_close_later(GtkWidget *splash, guint grace_ms)
{
    if (!GTK_IS_WIDGET(splash)) return;                                               // guard invalid pointer

    if (grace_ms == 0) {                                                              // immediate close requested?
        UmiSplash *s = g_object_get_data(G_OBJECT(splash), "umicom-splash-handle");   // get controller
        if (s) umi_splash_close(s);                                                   // close now
        return;                                                                       // done
    }

    /* Safety pattern:
     * Use g_timeout_add_full() and pass a strong ref to the widget as data,
     * with g_object_unref as destroy notify. This guarantees the pointer
     * remains valid until the callback fires, preventing UAF crashes.
     */                                                                              // explain safety pattern
    g_timeout_add_full(G_PRIORITY_DEFAULT,                                            // priority for timer
                       grace_ms,                                                      // delay in ms
                       umi_splash_close_timeout_cb,                                   // callback above
                       g_object_ref(splash),                                          // retain widget until fired
                       (GDestroyNotify)g_object_unref);                               // release after callback
}

/*-----------------------------------------------------------------------------
 * INTERNAL HELPERS
 *---------------------------------------------------------------------------*/  // private utilities

static void umi_splash_fill(UmiSplash *s, const char *title, const char *subtitle)
{
    /* Title is bold/bigger using Pango markup (local, no CSS file required). */      // styling approach
    if (title && *title) {                                                            // title provided?
        gchar *markup = g_markup_printf_escaped("<span size='12000' weight='bold'>%s</span>", title); // build markup safely
        gtk_label_set_markup(GTK_LABEL(s->title), markup);                            // apply markup to title
        g_free(markup);                                                               // free temporary string
    } else {                                                                          // otherwise use default
        gtk_label_set_text(GTK_LABEL(s->title), "Umicom Studio IDE");                 // fallback text
    }

    gtk_label_set_text(GTK_LABEL(s->subtitle), (subtitle && *subtitle) ? subtitle : ""); // set or clear subtitle
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(s->bar), 0.1);                     // small initial progress
}

static void umi_splash_try_load_png_logo(UmiSplash *s)
{
    /* GTK4-safe image loading from raw PNG bytes.
     * We prefer GdkTexture-from-bytes instead of legacy GdkPixbuf paths,
     * which avoids API confusion and works cleanly on GTK4.
     */                                                                              // API choice explanation
    size_t               png_sz = 0;                                                 // will hold byte length
    const unsigned char *png    = umi_splash_png(&png_sz);                           // get bytes from provider
    if (!png || png_sz == 0) {                                                       // no image available?
        gtk_widget_set_visible(s->image, FALSE);                                     // hide image widget
        return;                                                                      // nothing else to do
    }

    GError *err = NULL;                                                              // error accumulator
    GBytes *bytes = g_bytes_new_static(png, (gsize)png_sz);                          // wrap raw bytes in GBytes

    /* Create a texture directly from the PNG bytes. */                              // decode PNG → texture
    GdkTexture *tx = gdk_texture_new_from_bytes(bytes, &err);                        // attempt decode

    if (!tx) {                                                                       // decoding failed?
        if (err) { g_error_free(err); }                                              // drop error details (optional log)
        g_bytes_unref(bytes);                                                        // release byte wrapper
        gtk_widget_set_visible(s->image, FALSE);                                     // hide image and continue
        return;                                                                      // done
    }

    /* Optionally: scale within a reasonable bound by wrapping in GtkPicture
     * or just let GTK size it naturally. Keeping it simple: set the texture.
     */                                                                              // sizing policy
    gtk_image_set_from_paintable(GTK_IMAGE(s->image), GDK_PAINTABLE(tx));            // set decoded texture
    gtk_widget_set_visible(s->image, TRUE);                                          // ensure image is visible

    g_object_unref(tx);                                                              // unref temporary texture
    g_bytes_unref(bytes);                                                            // unref byte container
}

static gboolean umi_splash_do_autoclose(gpointer data)
{
    UmiSplash *s = (UmiSplash*)data;                                                 // recover controller
    s->auto_close_id = 0;                                                            // clear active timer token
    umi_splash_close(s);                                                             // close the window
    return G_SOURCE_REMOVE;                                                          // one-shot: remove source
}

/*---------------------------------------------------------------------------*/
/*  END OF FILE                                                               */
/*---------------------------------------------------------------------------*/
