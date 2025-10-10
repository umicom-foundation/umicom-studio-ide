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

/* Forward declarations only — actual implementations are in gui/llm_lab.c */
GtkWidget *umi_llm_lab_new(GtkWindow *parent);
void       umi_llm_lab_present(GtkWidget *win);

G_END_DECLS
#endif /* UMICOM_LLM_LAB_H */
