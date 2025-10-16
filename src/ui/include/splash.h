/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ui/include/splash.h
 *
 * PURPOSE (What is this file?):
 *   This header declares a tiny, *loosely-coupled* splash screen helper for
 *   the Umicom Studio IDE. It provides a small, brandable window that can be
 *   shown while the main application is loading (creating widgets, parsing
 *   settings, etc.). The API is intentionally minimal and depends only on GTK.
 *
 * WHY A SPLASH SCREEN?
 *   - Gives immediate visual feedback that the app launched successfully.
 *   - Lets you show your brand (logo/name) and short status messages.
 *   - Makes long initializations feel faster because users can "see progress".
 *
 * LOOSE COUPLING (very important to our architecture):
 *   - This module does NOT include or depend on any Umicom IDE internals.
 *   - It exposes a small C API around forward-declared GTK types.
 *   - Callers only pass primitive C strings or a parent GtkWindow pointer.
 *   - Internally we manage our own GTK widgets; callers don't need to know.
 *
 * HOW TO USE (copy-paste friendly, step-by-step):
 *   1) Create the splash *before* you do heavy work: 
 *
 *
 *        #include "splash.h"
 *
 *   2) Create the splash *before* you do heavy work:
 *
 *        UmiSplash *s = umi_splash_new(
 *            "Umicom Studio IDE",               // main title
 *            "Loading workspace and UI…",       // subtitle/initial message
 *            0                                  // auto-close ms (0 = no auto-close)
 *        );
 *
 *   2) Show it (optionally with a parent GtkWindow*; NULL is fine too):
 *
 *        umi_splash_show(s, /. parent = ./ NULL);
 *
 *   3) Update progress during initialization (fraction: 0.0..1.0):
 *
 *        umi_splash_set_progress(s, 0.25, "Initializing subsystems…");
 *        umi_splash_set_progress(s, 0.50, "Loading plugins…");
 *        umi_splash_set_progress(s, 0.75, "Preparing UI…");
 *
 *   4) When your main window is ready, close & free the splash:
 *
 *        umi_splash_close(s);   // hides the window (no-op if already hidden)
 *        umi_splash_free(s);    // releases resources (safe to call once)
 *
 * BRANDING HOOKS:
 *   - The implementation (splash.c) draws a simple branded header and supports
 *     showing a small in-memory logo icon if available (via icon.c). If you
 *     don't ship icon assets yet, the splash still works: it just shows text.
 *
 * ACCESSIBILITY:
 *   - We set titles and labels as normal GTK widgets so screen readers can
 *     announce them. Progress messages should be short and descriptive.
 *
 * THREADING:
 *   - Call these APIs from the GTK main thread (the usual rule with GTK).
 *   - If you have a worker thread, marshal updates to main via g_idle_add().
 *
 * LICENSE:
 *   MIT (see project root). Keep this header comment intact for attribution.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-15
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_IDE_SPLASH_H
#define UMICOM_STUDIO_IDE_SPLASH_H

/*-----------------------------------------------------------------------------
 * INCLUDES (What libraries we use here?)
 * - We only depend on GTK (gtk/gtk.h). This keeps the module independent of
 *   the rest of the code base. If GTK headers move in the future, only this
 *   include line needs adjusting.
 *---------------------------------------------------------------------------*/
#include <gtk/gtk.h>   /* GTK types (GtkWindow, GtkWidget) and functions */
#include <glib.h>
#include <stddef.h>    /* size_t for umi_splash_png */

G_BEGIN_DECLS

/*-----------------------------------------------------------------------------
 * PNG Resource Access
 * Returns pointer to embedded splash PNG bytes and size.
 * On Windows: reads from RCDATA resource IDP_SPLASH
 * On other platforms: returns compiled resource or NULL
 *---------------------------------------------------------------------------*/
const unsigned char *umi_splash_png(size_t *out_size);

/*-----------------------------------------------------------------------------
 * Backwards-Compatible API (for existing callers)
 * These wrap the internal UmiSplash implementation for easy migration.
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * OPAQUE TYPE (Why opaque?):
 * - We forward-declare a struct so callers can hold a pointer without knowing
 *   its internals. This is a classic C technique for information hiding and
 *   loose coupling: implementation details can change without breaking users.
 *---------------------------------------------------------------------------*/
typedef struct UmiSplash UmiSplash;

/*-----------------------------------------------------------------------------
 * umi_splash_new
 *
 * PURPOSE:
 *   Create a new splash-screen controller object (not yet visible). The splash
 *   is a small top-level window that shows:
 *     - A bold title (e.g., "Umicom Studio IDE")
 *     - A secondary subtitle/status under it (e.g., "Loading …")
 *     - A progress bar and a spinner (spinner shows activity even at 0%)
 *
 * PARAMETERS:
 *   title        : const char*  – Main heading on the splash (UTF-8 expected).
 *   subtitle     : const char*  – Smaller line beneath the title (UTF-8).
 *   auto_close_ms: unsigned int – If > 0, the splash will auto-close after the
 *                                  given number of milliseconds. Pass 0 to keep
 *                                  it open until you call umi_splash_close().
 *
 * RETURNS:
 *   UmiSplash*  – A heap-allocated controller. Use umi_splash_free() when done.
 *
 * NOTES:
 *   - This does *not* present the window yet. Call umi_splash_show() next.
 *   - Strings are copied internally; you can free your originals if needed.
 *---------------------------------------------------------------------------*/
UmiSplash* umi_splash_new(const char *title,
                          const char *subtitle,
                          unsigned int auto_close_ms);

/*-----------------------------------------------------------------------------
 * umi_splash_show
 *
 * PURPOSE:
 *   Present the splash window to the user. If you pass a parent GtkWindow,
 *   the splash will be set transient for it (keeps z-order above parent).
 *
 * PARAMETERS:
 *   splash : UmiSplash*   – The controller returned by umi_splash_new().
 *   parent : GtkWindow*   – (Optional) parent window for transient behavior.
 *                           Pass NULL if you don't have a main window yet.
*
 * RETURNS:
 *   void – no return value.
 *
 * USAGE:
 *   UmiSplash *s = umi_splash_new("App", "Starting…", 0);
 *   umi_splash_show(s, NULL);
 *---------------------------------------------------------------------------*/
void umi_splash_show(UmiSplash *splash, GtkWindow *parent);

/*-----------------------------------------------------------------------------
 * umi_splash_set_progress
 *
 * PURPOSE:
 *   Update the progress bar (0.0 to 1.0) and the message line beneath it.
 *   The function also nudges the spinner to reassure users that the app is
 *   alive, even if the progress fraction doesn't change (e.g., network wait).
 *
 * PARAMETERS:
 *   splash   : UmiSplash*  – valid controller
 *   fraction : double      – progress in [0.0, 1.0]. Values are clamped.
 *   message  : const char* – short status text (UTF-8). Can be NULL to keep
 *                             the previous message unchanged.
 *
 * RETURNS:
 *   void – no return value.
 *
 * TIP:
 *   Keep messages brief and task-based, e.g. "Indexing files…", "Loading UI…".
 *---------------------------------------------------------------------------*/
void umi_splash_set_progress(UmiSplash *splash,
                             double fraction,
                             const char *message);

/*-----------------------------------------------------------------------------
 * umi_splash_close
 *
 * PURPOSE:
 *   Hide the splash window if it is currently shown. This is safe to call
 *   multiple times; extra calls are simply ignored. The object remains valid
 *   after closing and can be freed by umi_splash_free().
*
 * RETURNS:
 *   void – no return value.
 *
 * WHEN TO CALL:
 *   When your main window is presented (e.g., after gtk_window_present()).
 *---------------------------------------------------------------------------*/
void umi_splash_close(UmiSplash *splash);

/*-----------------------------------------------------------------------------
 * umi_splash_free
 *
 * PURPOSE:
 *   Release all resources associated with the splash controller. This will
 *   also destroy the underlying GTK widgets if they still exist.
 *
 * RULE:
 *   Call this exactly once per object returned by umi_splash_new().
*
 * RETURNS:
 *   void – no return value.
 *---------------------------------------------------------------------------*/
void umi_splash_free(UmiSplash *splash);

/*-----------------------------------------------------------------------------
 * umi_splash_window
 *
 * PURPOSE:
 *   (Advanced) Retrieve the underlying GtkWindow*. Most callers don't need
 *   this, but it can be handy for tests or advanced theming tweaks.
 *
 * RETURNS:
 *   GtkWindow* – the toplevel splash window (owned by the controller).
 *---------------------------------------------------------------------------*/
GtkWindow *umi_splash_window(UmiSplash *splash);

/*-----------------------------------------------------------------------------
 * OPTIONAL: convenience inline to get GtkWidget* (sometimes handy for APIs).
 *---------------------------------------------------------------------------*/
static inline GtkWidget *umi_splash_widget(UmiSplash *s)
{
    return GTK_WIDGET(umi_splash_window(s));
}

/*=============================================================================
 * LEGACY COMPATIBILITY SHIM
 *-----------------------------------------------------------------------------
 * Some existing modules (e.g., src/gui/app/app.c) still call the older helper
 * names `uside_splash_show()` and `uside_splash_close_later()`. To keep this
 * build green *without touching those modules right now*, we declare
 * back-compat signatures here. They are implemented in splash.c in terms of
 * the modern UmiSplash controller. New code should prefer the umi_* API.
 *===========================================================================*/

/**
 * uside_splash_show:
 * @app: The GtkApplication instance
 * @auto_close_ms: Milliseconds before auto-closing (0 = manual close only)
 *
 * Creates and presents a splash window with the Umicom branding.
 * The window shows "Umicom Studio IDE" title, progress bar, and optional PNG.
 *
 * Returns: (transfer none): The GtkWidget* representing the splash window.
 *          DO NOT manually destroy this widget; use uside_splash_close_later().
 */
GtkWidget *uside_splash_show(GtkApplication *app, guint auto_close_ms);

/**
 * uside_splash_close_later:
 * @splash: The GtkWidget* returned by uside_splash_show()
 * @grace_ms: Grace period in milliseconds before closing (0 = immediate)
 *
 * Schedules the splash window to close after a grace period.
 * Safe to call multiple times; only the last call takes effect.
 */
void uside_splash_close_later(GtkWidget *splash, guint grace_ms);

G_END_DECLS
/*=============================================================================
 * LEGACY COMPATIBILITY SHIM
 *-----------------------------------------------------------------------------
 * Some existing modules (e.g., src/gui/app/app.c) still call the older helper
 * names `uside_splash_show()` and `uside_splash_close_later()`. To keep this
 * build green *without touching those modules right now*, we declare
 * back-compat signatures here. They are implemented in splash.c in terms of
 * the modern UmiSplash controller. New code should prefer the umi_* API.
 *===========================================================================*/


#endif /* UMICOM_STUDIO_IDE_SPLASH_H */