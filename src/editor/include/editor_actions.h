/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/editor/editor_actions.h
 * PURPOSE: Helpers for editor operations used by UI and menus.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_ACTIONS_H
#define UMICOM_EDITOR_ACTIONS_H

#include <editor.h>

/* Open a file path (errors via GError) */
gboolean umi_editor_open_file(UmiEditor *ed, const char *path, GError **err);

/* Save current buffer to current_file (if any). */
gboolean umi_editor_save(UmiEditor *ed, GError **err);

/* Save-As wrapper expected by other modules (dialog not yet implemented).
 * Returns FALSE with G_IO_ERROR_NOT_SUPPORTED if no path chooser is provided. */
gboolean umi_editor_save_as(UmiEditor *ed, GError **err);

/* Internal: Save to an explicit path (used by callers that already have a path). */
gboolean umi_editor_save_as_path(UmiEditor *ed, const char *path, GError **err);

/* Create a new untitled buffer. */
void     umi_editor_new_file(UmiEditor *ed);

#endif /* UMICOM_EDITOR_ACTIONS_H */
