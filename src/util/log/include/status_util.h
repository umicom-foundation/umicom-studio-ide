/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/status_util.h
 * PURPOSE: Minimal status helper used by editor/actions
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STATUS_UTIL_H
#define UMICOM_STATUS_UTIL_H

#include <gtk/gtk.h>

typedef struct _UmiStatus {
  GtkWidget *root; /* GtkBox with a single GtkLabel for simplicity */
  GtkLabel  *label;
} UmiStatus;

/* Create a small status line widget. */
UmiStatus *umi_status_new(void);

/* Access widget for packing. */
GtkWidget *umi_status_widget(UmiStatus *s);

/* Set and flash helpers. */
void umi_status_set  (UmiStatus *s, const char *text);
void umi_status_flash(UmiStatus *s, const char *text, guint msec);

#endif /* UMICOM_STATUS_UTIL_H */
