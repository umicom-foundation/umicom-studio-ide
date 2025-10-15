/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ui/resources/splash.c
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
#ifndef UMICOM_STUDIO_IDE_SPLASH_H
typedef struct UmiSplash UmiSplash;  /* Opaque to users; defined privately here */
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
struct UmiSplash {
    GtkWindow  *window;           /* The splash top-level window               */
    GtkWidget  *title_label;      /* Label used for the big, bold app title   */
    GtkWidget  *subtitle_label;   /* Label used for a smaller status message  */
    GtkWidget  *progress_bar;     /* Progress bar widget (0.0..1.0 fraction)  */
    GtkWidget  *spinner;          /* Activity spinner widget                   */
    unsigned int auto_close_ms;   /* Milliseconds to auto-close (0 = disabled)*/
    guint        auto_close_id;   /* g_timeout_add() id (0 means not active)  */
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
 *   x : double — value to clamp
 *
 * RETURNS:
 *   double — clamped value in [0.0, 1.0]
 *---------------------------------------------------------------------------*/
static inline double clamp_01(double x) {
    /* If x is less than 0.0, return 0.0; if greater than 1.0, return 1.0.   */
    if (x < 0.0) return 0.0;
    if (x > 1.0) return 1.0;
    return x;  /* Otherwise return x unchanged.                              */
}

/*-----------------------------------------------------------------------------
 * HELPER: on_timeout_auto_close
 *
 * PURPOSE:
 *   GLib timeout callback to auto-close the splash window after a delay.
 *   We use g_timeout_add() to schedule this once the splash is shown.
 *
 * PARAMETERS:
 *   data : gpointer — actually a UmiSplash* we provided when scheduling
 *
 * RETURNS:
 *   gboolean — return G_SOURCE_REMOVE to stop the timeout from repeating.
 *
 * SIDE EFFECTS:
 *   Calls umi_splash_close() to hide the splash. Also clears auto_close_id.
 *---------------------------------------------------------------------------*/
static gboolean on_timeout_auto_close(gpointer data) {
    /* Cast the generic pointer back to our UmiSplash type.                  */
    UmiSplash *s = (UmiSplash *)data;

    /* Safety check: only proceed if we have a valid controller and window.  */
    if (!s || !s->window) {
        return G_SOURCE_REMOVE;   /* Stop the timer; there is nothing to do. */
    }

    /* We call the public API close to centralize behavior in one place.     */
    /* This hides the window (if visible) and stops the spinner animation.   */
    /* It does *not* free the controller; freeing is explicit by the caller. */
    extern void umi_splash_close(UmiSplash *);  /* forward-declare prototype  */
    umi_splash_close(s);

    /* Clear the stored source id to signal "no active timer" from now on.   */
    s->auto_close_id = 0;

    /* Returning G_SOURCE_REMOVE tells GLib this timeout should not repeat.   */
    return G_SOURCE_REMOVE;
}

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
 *   s        : UmiSplash*   — controller to fill with widgets
 *   title    : const char*  — big title string (UTF-8)
 *   subtitle : const char*  — small subtitle string (UTF-8)
 *
 * RETURNS:
 *   void — widgets are created and assigned into `s`.
 *
 * NOTE:
 *   All widgets are referenced by the toplevel window; later we unref them
 *   by unref-ing the window in umi_splash_free().
 *---------------------------------------------------------------------------*/
static void build_ui(UmiSplash *s, const char *title, const char *subtitle) {
    /* Create the top-level window for the splash screen.                    */
    /* gtk_window_new() creates an empty toplevel; we will pack children.    */
    s->window = GTK_WINDOW(gtk_window_new());

    /* Give the window a minimal, compact size so it doesn’t feel bloated.   */
    gtk_window_set_default_size(s->window, 420, 240);

    /* Disable resizing: a splash should be a small, fixed-size window.      */
    gtk_window_set_resizable(s->window, FALSE);

    /* Center the splash on the active monitor for a polished appearance.    */
    gtk_window_set_position(s->window, GTK_WINDOW_POSITION_CENTER);

    /* Create a vertical box to stack elements from top to bottom.           */
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8 /* spacing */);

    /* Add margins so content doesn’t hug the edges; looks more refined.     */
    gtk_widget_set_margin_top(vbox,    16);
    gtk_widget_set_margin_bottom(vbox, 16);
    gtk_widget_set_margin_start(vbox,  20);
    gtk_widget_set_margin_end(vbox,    20);

    /* Create the big title label (e.g., “Umicom Studio IDE”).               */
    s->title_label = gtk_label_new(title ? title : "Umicom Studio IDE");

    /* Make title larger and bold by using CSS-style attributes.             */
    /* GTK4’s gtk_label_set_xalign() aligns text within its allocated space. */
    gtk_label_set_xalign(GTK_LABEL(s->title_label), 0.5f /* center */);

    /* Use markup for bold/size to avoid shipping a custom CSS file here.    */
    /* Pango markup: <span size='xx-large' weight='bold'>Title</span>        */
    {
        const char *text = title ? title : "Umicom Studio IDE";
        char *markup = g_markup_printf_escaped(
            "<span size='xx-large' weight='bold'>%s</span>", text);
        gtk_label_set_markup(GTK_LABEL(s->title_label), markup);
        g_free(markup);
    }

    /* Create the subtitle/status label to display short progress messages.   */
    s->subtitle_label = gtk_label_new(subtitle ? subtitle : "Starting…");

    /* Align subtitle to center as well; this is a splash, not a form.       */
    gtk_label_set_xalign(GTK_LABEL(s->subtitle_label), 0.5f);

    /* Add a small top margin to separate subtitle from title visually.      */
    gtk_widget_set_margin_top(s->subtitle_label, 4);

    /* Create a horizontal box to place spinner on the left and progress bar */
    /* to the right, giving a compact “activity + progress” layout.          */
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8 /* spacing */);

    /* Create the spinner to show ongoing activity.                          */
    s->spinner = gtk_spinner_new();

    /* The spinner is animated when started; we start it in show().          */
    /* Here we just create and pack it into the horizontal row.              */
    gtk_box_append(GTK_BOX(hbox), s->spinner);

    /* Create the progress bar; we will set its fraction via API calls.      */
    s->progress_bar = gtk_progress_bar_new();

    /* Set an initial fraction (0%) to make it visible and determinate.      */
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(s->progress_bar), 0.0);

    /* Expand the progress bar to fill available horizontal space.           */
    gtk_widget_set_hexpand(s->progress_bar, TRUE);

    /* Pack the progress bar to the right of the spinner.                    */
    gtk_box_append(GTK_BOX(hbox), s->progress_bar);

    /* Now pack everything into the vertical stack: title, subtitle, row.    */
    gtk_box_append(GTK_BOX(vbox), s->title_label);
    gtk_box_append(GTK_BOX(vbox), s->subtitle_label);
    gtk_box_append(GTK_BOX(vbox), hbox);

    /* Finally, set the vertical box as the content of the splash window.    */
    gtk_window_set_child(s->window, vbox);
}

/*-----------------------------------------------------------------------------
 * umi_splash_new
 *
 * PURPOSE:
 *   Allocate and create a new splash controller and build its UI. The window
 *   is created but not yet shown; call umi_splash_show() to present it.
 *---------------------------------------------------------------------------*/
UmiSplash* umi_splash_new(const char *title,
                          const char *subtitle,
                          unsigned int auto_close_ms)
{
    /* Allocate zero-initialized memory for the controller (safe defaults).  */
    UmiSplash *s = g_new0(UmiSplash, 1);

    /* Store the requested auto-close timeout (0 means “no auto-close”).     */
    s->auto_close_ms = auto_close_ms;

    /* Build the hierarchy of GTK widgets and assign them to our struct.     */
    build_ui(s, title, subtitle);

    /* Return the newly created controller to the caller.                    */
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
        gtk_window_set_transient_for(splash->window, parent);
        /* Mark as modal so clicks go to splash while it is up (optional).   */
        gtk_window_set_modal(splash->window, TRUE);
    }

    /* Start the spinner so users see continuous activity immediately.       */
    gtk_spinner_start(GTK_SPINNER(splash->spinner));

    /* Present the window to the user (GTK4 best-practice for toplevel).     */
    gtk_window_present(splash->window);

    /* If an auto-close ms was requested and no timer is active yet…         */
    if (splash->auto_close_ms > 0 && splash->auto_close_id == 0) {
        /* Schedule a one-shot timeout to close the splash after N ms.       */
        splash->auto_close_id = g_timeout_add(
            splash->auto_close_ms,         /* delay in milliseconds         */
            (GSourceFunc)on_timeout_auto_close,  /* callback function        */
            splash                         /* user data passed to callback */
        );
    }
}

/*-----------------------------------------------------------------------------
 * umi_splash_set_progress
 *
 * PURPOSE:
 *   Update splash progress and (optionally) its short status message.
 *---------------------------------------------------------------------------*/
void umi_splash_set_progress(UmiSplash *splash,
                             double fraction,
                             const char *message)
{
    /* If splash is invalid, do nothing; this makes call sites simpler.      */
    if (!splash || !splash->window) return;

    /* Clamp fraction to [0.0, 1.0] so GTK never sees out-of-range values.   */
    const double f = clamp_01(fraction);

    /* Set the normalized fraction on the GTK progress bar.                  */
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(splash->progress_bar), f);

    /* If a message is supplied (non-NULL and not empty), update subtitle.   */
    if (message && message[0] != '\0') {
        gtk_label_set_text(GTK_LABEL(splash->subtitle_label), message);
    }

    /* Nudge the spinner visually; if it wasn't running, start it now.       */
    /* (Starting twice is harmless; GTK tracks state internally.)            */
    gtk_spinner_start(GTK_SPINNER(splash->spinner));
}

/*-----------------------------------------------------------------------------
 * umi_splash_close
 *
 * PURPOSE:
 *   Hide/close the splash window. Safe to call multiple times. Also stops
 *   spinner animation and cancels any pending auto-close timeout.
 *---------------------------------------------------------------------------*/
void umi_splash_close(UmiSplash *splash)
{
    /* Defensive programming: ignore NULL to keep call sites minimal.        */
    if (!splash || !splash->window) return;

    /* Stop the spinner animation so CPU/GPU isn’t wasted after close.       */
    gtk_spinner_stop(GTK_SPINNER(splash->spinner));

    /* If the window is still visible, request to close it gracefully.       */
    /* gtk_window_close() is the GTK4 way to close a toplevel (emits signal).*/
    gtk_window_close(splash->window);

    /* If we had scheduled an auto-close timeout, cancel it now to be tidy.  */
    if (splash->auto_close_id != 0) {
        g_source_remove(splash->auto_close_id);
        splash->auto_close_id = 0;
    }
}

/*-----------------------------------------------------------------------------
 * umi_splash_free
 *
 * PURPOSE:
 *   Destroy the GTK window (which owns all child widgets) and free the
 *   controller memory. Call exactly once when you no longer need the splash.
 *---------------------------------------------------------------------------*/
void umi_splash_free(UmiSplash *splash)
{
    /* NULL is allowed; it’s a no-op (makes cleanup code simpler).           */
    if (!splash) return;

    /* If there is a scheduled timeout, remove it first to avoid callbacks   */
    /* firing after we free the underlying memory (a classic pitfall).       */
    if (splash->auto_close_id != 0) {
        g_source_remove(splash->auto_close_id);
        splash->auto_close_id = 0;
    }

    /* If the window exists, release our reference; this destroys the window */
    /* because the toplevel was created only for this splash and not shared. */
    if (splash->window) {
        /* In this project, we define gtk_widget_destroy=g_object_unref via  */
        /* a compile definition, but calling g_object_unref() directly is    */
        /* explicit and clear for new readers.                               */
        g_object_unref(splash->window);
        splash->window = NULL;
    }

    /* Finally free the controller struct itself.                            */
    g_free(splash);
}

/*-----------------------------------------------------------------------------
 * umi_splash_window
 *
 * PURPOSE:
 *   Return the underlying GtkWindow* for advanced callers who need to tweak
 *   presentation (e.g., set transient parent after creation, apply CSS, etc.)
 *---------------------------------------------------------------------------*/
GtkWindow* umi_splash_window(UmiSplash *splash)
{
    /* Return the pointer (may be NULL if not constructed properly).         */
    return splash ? splash->window : NULL;
}

/*-----------------------------------------------------------------------------
 * APPENDIX: Tips for Branding and Integration (kept as comments for teaching)
 *
 * - Showing a logo:
 *     This module intentionally avoids *requiring* a separate icon library to
 *     keep coupling low. If you want to show a small logo above the title:
 *       1) Add a GtkImage above `s->title_label` (in build_ui()).
 *       2) Create a GdkTexture from in-memory bytes (see icon.c helpers).
 *       3) Call gtk_image_set_from_paintable(image, GDK_PAINTABLE(texture)).
 *     That code can be added later without changing the public API.
 *
 * - Theming:
 *     To change colors or fonts, you can load a small CSS snippet at runtime
 *     (GtkCssProvider) and apply it to the display. We skip CSS here to keep
 *     the splash self-contained. The markup we used for the title keeps it
 *     simple and portable.
 *
 * - Auto-close vs manual close:
 *     We support both. If `auto_close_ms` is 0, you have to call
 *     umi_splash_close() yourself (typical when you close it right after
 *     presenting your fully built main window). If non-zero, it will close
 *     on a timer — useful for demos or very fast-starting apps.
 *
 * - Threaded initialization:
 *     Keep the splash on the GTK thread. If background threads run work,
 *     periodically `g_idle_add()` back to main to call umi_splash_set_progress().
 *
 * - Windows executable icon:
 *     You can embed a .ico via a .rc file (see CMake comments in CMakeLists).
 *     At runtime, the splash icon follows the application icon (theme/OS).
 *
 * - Why we call gtk_window_close() then unref in free():
 *     Close hides the window and emits signals. Unref in free() releases
 *     the last reference and destroys the widget tree. Doing both is a
 *     safe pattern and teaches newcomers the difference between “closing”
 *     (user-visible behavior) and “destroying” (memory management).
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------