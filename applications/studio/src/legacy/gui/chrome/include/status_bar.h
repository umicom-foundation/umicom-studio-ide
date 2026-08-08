/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/chrome/include/status_bar.h
 *
 * PURPOSE:
 *   Public API for a minimal, safe status bar used by the chrome module.
 *   The implementation lives in src/gui/chrome/status_bar.c.
 *
 * DESIGN:
 *   - Opaque handle (UmiStatusBar) with a single visible GtkWidget*.
 *   - GTK4-only (GtkBox + GtkLabel internally).
 *   - Thread-agnostic: all calls should be made from the GTK main thread.
 *
 * API:
 *   typedef struct _UmiStatusBar UmiStatusBar;
 *   UmiStatusBar *umi_status_bar_new(void);
 *   GtkWidget    *umi_status_bar_widget(UmiStatusBar *sb);   // pack this
 *   void          umi_status_bar_set  (UmiStatusBar *sb, const char *text);
 *   void          umi_status_bar_flash(UmiStatusBar *sb, const char *text, guint ms);
 *   void          umi_status_bar_free (UmiStatusBar *sb);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef USIDE_STATUS_BAR_H
#define USIDE_STATUS_BAR_H

#include <gtk/gtk.h>
#include <glib.h>

G_BEGIN_DECLS

typedef struct _UmiStatusBar UmiStatusBar;

UmiStatusBar *umi_status_bar_new(void);
GtkWidget    *umi_status_bar_widget(UmiStatusBar *sb);
void          umi_status_bar_set  (UmiStatusBar *sb, const char *text);
void          umi_status_bar_flash(UmiStatusBar *sb, const char *text, guint ms);
void          umi_status_bar_free (UmiStatusBar *sb);

G_END_DECLS

#endif /* USIDE_STATUS_BAR_H */
