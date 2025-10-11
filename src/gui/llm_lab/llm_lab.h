/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: SYSTEM/umicom/llm_lab.h
 * PURPOSE: Minimal forward declarations used by gui/llm_lab.c so the file
 *          compiles on Windows where headers may be arranged differently.
 *          This fixes: fatal error: umicom/llm_lab.h: No such file or directory
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-09 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LLM_LAB_H
#define UMICOM_LLM_LAB_H

#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

/* 
 * Factory with explicit parent:
 * Creates the LLM Lab widget. If @parent is non-NULL the lab may
 * use it for modality/transient relationships when presented.
 */
GtkWidget *umi_llm_lab_new_with_parent (GtkWindow *parent);

/*
 * Convenience no-arg factory:
 * Historically some code called umi_llm_lab_new(void).
 * We keep source compatibility by providing a macro that calls
 * umi_llm_lab_new_with_parent(NULL).
 */
#define umi_llm_lab_new()  umi_llm_lab_new_with_parent(NULL)

/*
 * Present helper:
 * Presents/shows the lab window/pane associated with @w (if applicable),
 * or brings it to the front if already visible.
 */
void        umi_llm_lab_present          (GtkWidget *w);

G_END_DECLS

#endif /* UMI_LLM_LAB_H */
