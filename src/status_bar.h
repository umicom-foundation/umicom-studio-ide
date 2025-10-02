/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/status_bar.h
 * PURPOSE: Composite status bar with label + activity spinner
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STATUS_BAR_H
#define UMICOM_STATUS_BAR_H

#include <gtk/gtk.h>

/* Opaque handle */
typedef struct _UmiStatusBar UmiStatusBar;

UmiStatusBar *umi_status_bar_new(void);
GtkWidget    *umi_status_bar_widget(UmiStatusBar *sb);

/* Set permanent text. */
void          umi_status_bar_set(UmiStatusBar *sb, const char *text);

/* Flash text for 'msec' and then restore. */
void          umi_status_bar_flash(UmiStatusBar *sb, const char *text, guint msec);

/* Toggle the spinner (TRUE = spinning). */
void          umi_status_bar_busy(UmiStatusBar *sb, gboolean busy);

/* Destroy and free. */
void          umi_status_bar_free(UmiStatusBar *sb);

#endif /* UMICOM_STATUS_BAR_H */
