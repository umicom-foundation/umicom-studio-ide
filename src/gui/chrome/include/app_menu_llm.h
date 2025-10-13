/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/chrome/include/app_menu_llm.h
 *
 * PURPOSE:
 *   Public factory for a minimal “menu bar” row dedicated to LLM controls.
 *
 * API:
 *   typedef void (*UmiSimpleAction)(gpointer user);
 *   GtkWidget *umi_app_menu_llm_new(UmiSimpleAction on_save, gpointer user);
 *   void       umi_app_menu_llm_free(GtkWidget *bar);
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#pragma once
#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*UmiSimpleAction)(gpointer user);

/* Build a small bar with:
 *   [ LLM Lab ]             ...spacer...             [ Save ]
 * The Save button invokes `on_save(user)`.
 */
GtkWidget *umi_app_menu_llm_new(UmiSimpleAction on_save, gpointer user);

/* Destroy the bar and all its children (safe with NULL). */
void       umi_app_menu_llm_free(GtkWidget *bar);

#ifdef __cplusplus
}
#endif
