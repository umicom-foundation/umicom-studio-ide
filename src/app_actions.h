/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/app_actions.h
 * PURPOSE: Bridge callbacks for keymap to invoke app features
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APP_ACTIONS_H
#define UMICOM_APP_ACTIONS_H

#include "app.h"
#include "editor_actions.h"
#include "session.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Populate a UmiKeymapCallbacks suitable for umi_keymap_install(). */
void umi_app_fill_keymap(GtkApplication *app, UmiKeymapCallbacks *out);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UMICOM_APP_ACTIONS_H */
