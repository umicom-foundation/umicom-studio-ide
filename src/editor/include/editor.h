/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/editor/editor.h
 * PURPOSE: Public composite editor API and state.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *---------------------------------------------------------------------------*/
#pragma once
#include <gtk/gtk.h>

/* Forward declarations to keep this header light */
typedef struct _UmiOutputPane  UmiOutputPane;
typedef struct _UmiProblemList UmiProblemList;
typedef struct _UmiStatus      UmiStatus;

/* Public editor state used across the app.
 * Note: 'buffer', 'current_file', and 'status' are optional; actions check for NULL.
 */
typedef struct _UmiEditor { 
  GtkWidget      *root;         /* top-level widget */
  UmiOutputPane  *out;          /* console/output pane */
  UmiProblemList *problems;     /* diagnostics/problems list */

  /* Minimal text state so open/save compile, even before GtkSourceView integration */
  GtkTextBuffer  *buffer;       /* detached text buffer (no view yet) */
  char           *current_file; /* full path of current file (or NULL) */
  UmiStatus      *status;       /* optional status object */
} UmiEditor;

UmiEditor *umi_editor_new(void);
GtkWidget *umi_editor_widget(UmiEditor *ed);
void       umi_editor_free(UmiEditor *ed);

/* File ops used elsewhere in the app */
gboolean umi_editor_open_file(UmiEditor *ed, const char *path, GError **err);
gboolean umi_editor_save     (UmiEditor *ed, GError **err);
gboolean umi_editor_save_as  (UmiEditor *ed, GError **err); /* wrapper (no dialog yet) */
