/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/app_menu_llm.c
 * PURPOSE: LLM menu wiring (implementation) — safe stubs
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-07 | MIT
 *---------------------------------------------------------------------------*/
#include "app_menu_llm.h"

/* NOTE:
   This file intentionally provides *no-op* implementations. It unblocks the
   build when the header is included from app.c (or elsewhere) but no LLM
   menus are needed yet. Replace these bodies with your actual GMenuModel
   wiring whenever you are ready.
*/

void umi_llm_menu_init(GtkApplication *app)      { (void)app; }
void umi_llm_menu_teardown(GtkApplication *app)  { (void)app; }
