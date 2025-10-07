/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/llm_lab.h
 * PURPOSE: Public interface for the optional LLM Lab dock/panel
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_LLM_LAB_H
#define UMI_LLM_LAB_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Create and return the LLM Lab widget (standalone panel). */
GtkWidget *umi_llm_lab_new(void);

G_END_DECLS

#endif /* UMI_LLM_LAB_H */
