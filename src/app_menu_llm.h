/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/app_menu_llm.h
 * PURPOSE: LLM menu wiring (header) — minimal no-op API to satisfy includes
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-07 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APP_MENU_LLM_H
#define UMICOM_APP_MENU_LLM_H

#include <gtk/gtk.h>

/* Initialize/teardown the "AI" (LLM) items in the application menu.
 * These are intentionally no-ops here so the project builds cleanly even
 * when the full feature isn't wired yet. You can expand later without
 * touching call sites. */
void umi_llm_menu_init(GtkApplication *app);
void umi_llm_menu_teardown(GtkApplication *app);

#endif /* UMICOM_APP_MENU_LLM_H */
