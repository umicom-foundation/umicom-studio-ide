#ifndef UMICOM_LLM_LAB_H
#define UMICOM_LLM_LAB_H
/* Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT */
#include <gtk/gtk.h>

/* Create the LLM Lab widget (dockable). */
GtkWidget* umi_llm_lab_new(void);

/* Optional: focus if already present in a notebook/sidebar. */
void umi_llm_lab_focus(GtkWidget *lab);

#endif
