/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/app_menu_llm.h
 * PURPOSE: Application menu bar (/LLM) factory for the main window
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#pragma once
#include <gtk/gtk.h>

/* Simple callback used for menu items that just call back into the app. */
typedef void (*UmiSimpleAction)(gpointer user);

/* Build a small menubar-like row with basic items (Save, LLM Lab).
 * 'on_save' is invoked when the Save item is activated.
 * 'user' is passed back to the callback (typically your UmiApp*).
 */
GtkWidget *umi_app_menu_llm_new(UmiSimpleAction on_save, gpointer user);
