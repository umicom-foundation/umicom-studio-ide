/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/editor/editor.h
 * PURPOSE: See file-specific notes below
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#pragma once
#include <gtk/gtk.h>
#include "output_pane.h"
#include "problem_list.h"

/* PURPOSE (editor.h):
 * Public definition of the editor composite so other modules can access
 * 'root' directly (as your code already does). The editor provides an
 * output pane and a problems list at the bottom, with a placeholder
 * editor content on top (you can later replace with your actual editor).
 */

typedef struct _UmiEditor {
  GtkWidget     *root;      /* top-level widget for this editor */
  UmiOutputPane *out;       /* console/output pane */
  UmiProblemList *problems; /* diagnostics/problems list */
} UmiEditor;

UmiEditor *umi_editor_new(void);
GtkWidget *umi_editor_widget(UmiEditor *ed);
void       umi_editor_free(UmiEditor *ed);

/* File ops used elsewhere in the app (no-op stubs here so the app links) */
gboolean umi_editor_open_file(UmiEditor *ed, const char *path, GError **err);
gboolean umi_editor_save     (UmiEditor *ed, GError **err);
gboolean umi_editor_save_as  (UmiEditor *ed, GError **err);
