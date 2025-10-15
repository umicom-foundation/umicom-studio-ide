/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ui/include/splash.h
 *
 * PURPOSE (plain English):
 *   This header declares a tiny, re-usable “splash screen” helper that we can
 *   show while the main application starts.  It is intentionally simple and
 *   pure-C so that *new contributors* can read and understand it quickly.
 *
 * WHY A SEPARATE MODULE:
 *   - Loose coupling: other modules can use the splash screen by including
 *     this one header.  No cross-module private headers, no global state.
 *   - Small API surface: you only create, show, update text, and close.
 *
 * WHAT A "SPLASH" IS:
 *   A small undecorated window shown briefly during start-up that contains:
 *     - our brand glyph (from icon.c via icon.h) or a built-in fallback
 *     - the application name and an optional status line (“Loading…”)
 *     - a pulsing progress bar so users feel something is happening
 *
 * HOW TO USE (copy & paste):
 *   #include "splash.h"
 *
 *   static UmiSplash *g_splash = NULL;
 *
 *   // In your GtkApplication "startup" handler (very early):
 *   g_splash = umi_splash_new(app, 420, 260, "Umicom Studio IDE",
 *                             "Starting services…", 0 /*no auto hide*/);
 *   umi_splash_show(g_splash);  // non-blocking; the window appears centered
 *
 *   // If you have steps and want to update the subtitle:
 *   umi_splash_set_message(g_splash, "Loading settings…");
 *
 *   // Later, in "activate" when your main window is ready:
 *   umi_splash_hide_and_destroy(g_splash);  // closes + frees; safe if NULL
 *   g_splash = NULL;
 *
 *   // Optional: if you prefer, create with autohide in N milliseconds:
 *   // umi_splash_new(..., autohide_ms = 1500);
 *
 * COMPILATION:
 *   This module only depends on GTK and GLib (and optionally icon.h for a
 *   prettier glyph).  CMake just needs to compile splash.c and link to GTK.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-14
 * License: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_SPLASH_H                /* Standard include guard.          */
#define UMICOM_UI_SPLASH_H

/* We include GTK+GLib headers because our API exposes GtkApplication types.   */
#include <gtk/gtk.h>                      /* GtkApplication, GtkWindow, etc.  */

G_BEGIN_DECLS                              /* Friendly to C++ consumers.      */

/*-----------------------------------------------------------------------------
 * TYPE: UmiSplash
 *
 * We expose UmiSplash as an *opaque* struct (callers only see a pointer).
 * Hiding fields keeps the implementation private and avoids coupling.
 *---------------------------------------------------------------------------*/
typedef struct _UmiSplash UmiSplash;

/*-----------------------------------------------------------------------------
 * umi_splash_new
 *
 * PURPOSE:
 *   Allocate and initialize a splash window object.  You may show it
 *   immediately (umi_splash_show) or later.  If `autohide_ms > 0`, we schedule
 *   a GLib timeout to close it automatically after that delay.
 *
 * PARAMETERS:
 *   app          - your GtkApplication (so the window is application-owned).
 *   width_px     - preferred splash width  (e.g. 420).
 *   height_px    - preferred splash height (e.g. 260).
 *   title_text   - large title (typically product name).
 *   subtitle     - smaller line (e.g. “Loading…”); may be NULL.
 *   autohide_ms  - 0 to keep it open until you explicitly close; otherwise
 *                  number of milliseconds after which it closes itself.
 *
 * RETURNS:
 *   Pointer to a newly created UmiSplash object (never NULL on success).
 *---------------------------------------------------------------------------*/
UmiSplash* umi_splash_new(GtkApplication *app,
                          int width_px,
                          int height_px,
                          const char *title_text,
                          const char *subtitle,
                          guint autohide_ms);

/*-----------------------------------------------------------------------------
 * umi_splash_show
 *
 * PURPOSE:
 *   Present (show) the splash window centered on screen.  This is non-blocking
 *   — the GTK main loop continues.  Safe to call multiple times (no harm).
 *---------------------------------------------------------------------------*/
void       umi_splash_show(UmiSplash *sp);

/*-----------------------------------------------------------------------------
 * umi_splash_set_message
 *
 * PURPOSE:
 *   Update the splash subtitle line at runtime.  Passing NULL clears the text.
 *---------------------------------------------------------------------------*/
void       umi_splash_set_message(UmiSplash *sp, const char *message);

/*-----------------------------------------------------------------------------
 * umi_splash_hide_and_destroy
 *
 * PURPOSE:
 *   Close the splash window (if still open), stop any pending autohide timer,
 *   free all associated memory, and make the pointer invalid.  It is safe to
 *   call with NULL (we do nothing).
 *---------------------------------------------------------------------------*/
void       umi_splash_hide_and_destroy(UmiSplash *sp);

/*-----------------------------------------------------------------------------
 * umi_splash_window
 *
 * PURPOSE:
 *   Give callers access to the underlying GtkWindow* in case they want to
 *   tweak details (CSS, position, etc.).  The window remains owned by the
 *   splash object — do not unref it manually.
 *---------------------------------------------------------------------------*/
GtkWindow* umi_splash_window(UmiSplash *sp);

G_END_DECLS

#endif /* UMICOM_UI_SPLASH_H */
/*---------------------------------------------------------------------------*/