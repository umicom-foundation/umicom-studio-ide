/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/output_pane.h
 * PURPOSE: Simple output console widget with append APIs
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OUTPUT_PANE_H
#define UMICOM_OUTPUT_PANE_H

#include <gtk/gtk.h>

typedef struct _UmiOutputPane UmiOutputPane;

UmiOutputPane* umi_output_pane_new(void);
GtkWidget*     umi_output_pane_widget(UmiOutputPane *p);
void           umi_output_pane_clear(UmiOutputPane *p);
void           umi_output_pane_append(UmiOutputPane *p, const char *text);
void           umi_output_pane_append_line(UmiOutputPane *p, const char *text);
void           umi_output_pane_append_line_err(UmiOutputPane *p, const char *text);

#endif /* UMICOM_OUTPUT_PANE_H */
