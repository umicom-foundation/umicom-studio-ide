/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/actions.h
 * PURPOSE: Install a set of default GActions on the application
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_ACTIONS_H
#define UMICOM_ACTIONS_H

#include <gtk/gtk.h>

/* Install actions on 'app'. Call once during startup. */
void umi_actions_install(GtkApplication *app);

#endif /* UMICOM_ACTIONS_H */
