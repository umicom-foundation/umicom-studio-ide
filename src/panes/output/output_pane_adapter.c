/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/output_pane_adapter.c
 * PURPOSE: Implementation of the console adapter
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "output_pane_adapter.h"

UmiOutputPaneAdapter *umi_output_pane_adapter_new(void){
  UmiOutputPaneAdapter *a = g_new0(UmiOutputPaneAdapter,1);
  a->console = umi_output_console_new();
  return a;
}

GtkWidget *umi_output_pane_adapter_widget(UmiOutputPaneAdapter *a){
  if(!a) return NULL;
  return umi_output_console_widget(a->console);
}
