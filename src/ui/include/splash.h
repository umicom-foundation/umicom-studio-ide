/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ui/include/splash.h
 *
 * PURPOSE:
 *   Small public API for showing a splash window while the app starts up.
 *   Pure C, no XML. Uses a GdkTexture from PNG/SVG on disk if available,
 *   otherwise falls back to a tiny built-in XPM image.
 *
 * HOW TO USE:
 *   1) Include this header in app.c
 *   2) Call:
 *        GtkWidget *s = uside_splash_show(app, 600 ms);
 *        // ... init work ...
 *        uside_splash_close_later(s, 200 /'*ms grace*'/);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#pragma once

#include <gtk/gtk.h>

/* Show a non-modal splash window; returns the GtkWindow so you can close it.  */
GtkWidget *uside_splash_show(GtkApplication *app, guint display_ms);

/* Close the splash window after `grace_ms` (0 = immediate). Safe on NULL.     */
void uside_splash_close_later(GtkWidget *splash_win, guint grace_ms);
