/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/editor_actions.h
 * PURPOSE: Helpers for editor operations used by UI and menus
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_ACTIONS_H
#define UMICOM_EDITOR_ACTIONS_H

#include "editor.h"

/* Open a file path (errors appended to output). */
gboolean umi_editor_open_file(UmiEditor *ed, const char *path, GError **err);

/* Save current buffer to current_file (if any). */
gboolean umi_editor_save(UmiEditor *ed, GError **err);

/* Save as a new path. */
gboolean umi_editor_save_as(UmiEditor *ed, const char *path, GError **err);

/* Create a new untitled buffer. */
void     umi_editor_new_file(UmiEditor *ed);

#endif /* UMICOM_EDITOR_ACTIONS_H */
