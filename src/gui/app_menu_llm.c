/* App hook to open LLM Lab from menu */
#include "umicom/llm_lab.h"
#include <gtk/gtk.h>

/* This file expects you to wire an action/handler (ACTION_OPEN_LLM_LAB)
 * from your main window. For now we expose a helper that creates the panel.
 */
GtkWidget* umi_open_llm_lab_dock(GtkWidget *parent_container){
  GtkWidget *lab = umi_llm_lab_new();
  if(parent_container && GTK_IS_BOX(parent_container)){
    gtk_box_append(GTK_BOX(parent_container), lab);
  }
  return lab;
}
