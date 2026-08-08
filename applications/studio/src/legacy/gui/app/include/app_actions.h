/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/app/include/app_actions.h
 *
 * PURPOSE:
 *   Declarations for wiring high-level application actions into a central
 *   keymap. This header *depends on* the shared keymap API to avoid
 *   duplicate struct definitions across modules. Pure C; no CSS.
 *
 * API:
 *   void umi_app_fill_keymap(GtkApplication *app, UmiKeymapCallbacks *out);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_APP_ACTIONS_H
#define UMI_APP_ACTIONS_H

#include <gtk/gtk.h>
#include "keymap.h"   /* <- single source of truth for UmiKeymapCallbacks */

G_BEGIN_DECLS

void umi_app_fill_keymap(GtkApplication *app, UmiKeymapCallbacks *out);

G_END_DECLS
#endif /* UMI_APP_ACTIONS_H */
