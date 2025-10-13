/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/editor/editor_actions.h
 *
 * PURPOSE:
 *   Public helpers for common editor operations (open/save/new). These are
 *   used by menu items, toolbar actions, or keyboard shortcuts.
 *
 * DESIGN:
 *   - Declares only what UI and other modules need.
 *   - The implementation has no GUI dialogs yet (save-as is a stub).
 *
 * API:
 *   gboolean umi_editor_open_file(UmiEditor *ed, const char *path, GError **err);
 *   gboolean umi_editor_save     (UmiEditor *ed, GError **err);
 *   gboolean umi_editor_save_as  (UmiEditor *ed, GError **err);
 *   gboolean umi_editor_save_as_path(UmiEditor *ed, const char *path, GError **err);
 *   void     umi_editor_new_file (UmiEditor *ed);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_ACTIONS_H
#define UMICOM_EDITOR_ACTIONS_H

#include "editor.h"  /* UmiEditor, GtkTextBuffer */

gboolean umi_editor_open_file   (UmiEditor *ed, const char *path, GError **err);
gboolean umi_editor_save        (UmiEditor *ed, GError **err);
gboolean umi_editor_save_as     (UmiEditor *ed, GError **err);
gboolean umi_editor_save_as_path(UmiEditor *ed, const char *path, GError **err);
void     umi_editor_new_file    (UmiEditor *ed);

#endif /* UMICOM_EDITOR_ACTIONS_H */
