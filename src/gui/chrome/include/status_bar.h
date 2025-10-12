/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/chrome/include/status_bar.h
 * PURPOSE: Minimal status bar API for the chrome module (self-contained).
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/
#ifndef USIDE_STATUS_BAR_H
#define USIDE_STATUS_BAR_H

#include <gtk/gtk.h>
#include <glib.h>

G_BEGIN_DECLS

typedef struct _UmiStatusBar UmiStatusBar;

/* Create a new status bar instance. */
UmiStatusBar *umi_status_bar_new(void);

/* Retrieve the GTK widget to pack into layouts (do NOT free). */
GtkWidget    *umi_status_bar_widget(UmiStatusBar *sb);

/* Set persistent text on the bar (overwrites any flashing text). */
void          umi_status_bar_set(UmiStatusBar *sb, const char *text);

/* Show text temporarily for `ms` milliseconds, then clear back to empty. */
void          umi_status_bar_flash(UmiStatusBar *sb, const char *text, guint ms);

G_END_DECLS

#endif /* USIDE_STATUS_BAR_H */
/*--- end of file ---*/