/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/dialog/include/open_folder.h
 *
 * PURPOSE:
 *   Synchronous helper that prompts the user to select a folder, then updates
 *   the workspace root. Implemented with GTK4 GtkFileDialog + async->sync loop.
 *
 * DESIGN:
 *   - Includes headers by NAME only to keep modules loosely coupled.
 *   - CMake must add 'src/core/include' to target include dirs so
 *     "workspace.h" resolves without deep relative paths.
 *
 * API:
 *   void umi_open_folder_prompt(GtkWindow *parent, UmiWorkspace *ws);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_FOLDER_H
#define UMICOM_OPEN_FOLDER_H

#include <gtk/gtk.h>
#include "workspace.h"   /* <- ensure CMake adds src/core/include to includes */

void umi_open_folder_prompt(GtkWindow *parent, UmiWorkspace *ws);

#endif /* UMICOM_OPEN_FOLDER_H */
