/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/menu.h
 * PURPOSE: Application menu model
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MENU_H
#define UMICOM_MENU_H

#include <gtk/gtk.h>

/* Create and set a basic application menu bound to default actions. */
void umi_menu_install(GtkApplication *app);

#endif /* UMICOM_MENU_H */
