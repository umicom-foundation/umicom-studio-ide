/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/app_actions.h
 * PURPOSE: Fill the keyboard callback table (connects to App methods)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-07 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APP_ACTIONS_H
#define UMICOM_APP_ACTIONS_H

#include <gtk/gtk.h>
#include "keymap.h"

typedef struct _UmiApp UmiApp;

void umi_app_fill_keymap(GtkApplication *app, UmiKeymapCallbacks *out);

#endif /* UMICOM_APP_ACTIONS_H */
