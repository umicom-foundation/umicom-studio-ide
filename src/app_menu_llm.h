/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/app_menu_llm.h
 * PURPOSE: Simple "menu bar" strip for LLM actions used by the main app shell
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_APP_MENU_LLM_H
#define UMI_APP_MENU_LLM_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* A very small callback signature used by the app to hook "Save" etc. */
typedef void (*UmiSimpleAction)(gpointer user);

/* Build a lightweight top-strip "menu bar" with an LLM Lab button and Save.
 * This returns a GtkWidget that you can pack at the top of the window.
 */
GtkWidget *umi_app_menu_llm_new(UmiSimpleAction on_save, gpointer user_data);

G_END_DECLS

#endif /* UMI_APP_MENU_LLM_H */
