/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/open_folder.h
 * PURPOSE: Helper to prompt for a folder and set workspace root
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OPEN_FOLDER_H
#define UMICOM_OPEN_FOLDER_H

#include <gtk/gtk.h>
#include "../../../core/include/workspace.h"

void umi_open_folder_prompt(GtkWindow *parent, UmiWorkspace *ws);

#endif /* UMICOM_OPEN_FOLDER_H */
