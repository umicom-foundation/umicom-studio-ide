/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/editor.h
 * PURPOSE: Editor widget: text view + bottom notebook (Output, Problems)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_H
#define UMICOM_EDITOR_H

#include <gtk/gtk.h>
#include "output_pane.h"
#include "problem_list.h"
#include "status_util.h"

typedef struct _UmiEditor {
  GtkWidget *root;       /* vertical box: [scrolled text][notebook] */
  GtkTextView  *view;
  GtkTextBuffer *buffer;
  GtkWidget *scroller;
  GtkNotebook *bottom;
  UmiOutputPane *out;
  UmiProblemList *problems;
  UmiStatus *status;
  gchar *current_file;
} UmiEditor;

/* Construct the editor widget and its bottom notebook tabs. */
UmiEditor *umi_editor_new(void);

/* Utility: set caret to a specific position (line,col, 1-based). */
void umi_editor_jump(UmiEditor *ed, int line, int col);

/* Get widget for packing in the window. */
GtkWidget *umi_editor_widget(UmiEditor *ed);

#endif /* UMICOM_EDITOR_H */
