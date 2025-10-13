/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/llm_lab/include/llm_lab.h
 *
 * PURPOSE:
 *   Public header for the LLM Lab widget. The lab is designed to compile
 *   even when no LLM backend is linked (weak symbols in the .c provide
 *   graceful fallbacks).
 *
 * API:
 *   GtkWidget *umi_llm_lab_new_with_parent(GtkWindow *parent);
 *   #define    umi_llm_lab_new() umi_llm_lab_new_with_parent(NULL)
 *   void        umi_llm_lab_present(GtkWidget *lab_widget);
 *
 * DESIGN:
 *   - Include by NAME only; CMake must add this module's /include path.
 *   - Header contains no provider-specific types to keep coupling low.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LLM_LAB_H
#define UMICOM_LLM_LAB_H

#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

GtkWidget *umi_llm_lab_new_with_parent (GtkWindow *parent);
#define     umi_llm_lab_new()          umi_llm_lab_new_with_parent(NULL)
void        umi_llm_lab_present        (GtkWidget *w);

G_END_DECLS

#endif /* UMICOM_LLM_LAB_H */
