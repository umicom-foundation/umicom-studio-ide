/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/output_pane_adapter.h
 * PURPOSE: Bridge legacy UmiOutputPane API to UmiOutputConsole
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OUTPUT_PANE_ADAPTER_H
#define UMICOM_OUTPUT_PANE_ADAPTER_H

#include "output_console.h"
#include "output_pane.h" /* legacy API */

/* Adapt a UmiOutputConsole to present the same widget as legacy pane,
 * so existing code using umi_output_pane_* continues to compile. */
typedef struct {
  UmiOutputConsole *console;
} UmiOutputPaneAdapter;

UmiOutputPaneAdapter *umi_output_pane_adapter_new(void);
GtkWidget            *umi_output_pane_adapter_widget(UmiOutputPaneAdapter *a);

/* Legacy-style convenience wrappers */
static inline void umi_output_pane_adapter_append(UmiOutputPaneAdapter *a, const char *s){
  if(a && a->console) umi_output_console_append_line(a->console, s);
}
static inline void umi_output_pane_adapter_clear(UmiOutputPaneAdapter *a){
  if(a && a->console) umi_output_console_clear(a->console);
}

#endif /* UMICOM_OUTPUT_PANE_ADAPTER_H */
