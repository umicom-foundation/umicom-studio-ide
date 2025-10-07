/*----------------------------------------------------------------------------- 
 * Umicom Studio IDE
 * File: src/editor.h
 * PURPOSE: Public API + struct for the editor widget
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-07 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_EDITOR_H
#define UMI_EDITOR_H

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward decls for project types we hold by pointer */
typedef struct _UmiOutputPane   UmiOutputPane;
typedef struct _UmiProblemList  UmiProblemList;
typedef struct _UmiStatus       UmiStatus;

/* Public editor structure. This was previously opaque, which caused
 * 'invalid application of sizeof to an incomplete type' in editor.c.
 * Making it concrete here resolves that build error across the tree.
 */
typedef struct _UmiEditor {
  GtkWidget      *root;        /* top-level container to pack into UI */
  GtkTextView    *view;        /* main text view */
  GtkTextBuffer  *buffer;      /* buffer owned by view */
  GtkWidget      *scroller;    /* scroll container for text view */
  GtkNotebook    *bottom;      /* bottom notebook: Output / Problems */
  UmiOutputPane  *out;         /* output pane (logs/build output) */
  UmiProblemList *problems;    /* problems/diagnostics list */
  UmiStatus      *status;      /* status line helper */
  gchar          *current_file;/* last opened/saved file path */
} UmiEditor;

/* Construction */
UmiEditor *umi_editor_new(void);
/* Access to the main widget for packing into layouts */
GtkWidget *umi_editor_widget(UmiEditor *ed);
/* Navigation helper */
void umi_editor_jump(UmiEditor *ed, int line, int col);

/* File operations */
void umi_editor_open_file(UmiEditor *ed, const char *path, GError **err);
void umi_editor_save     (UmiEditor *ed, GError **err);
void umi_editor_save_as  (UmiEditor *ed, GError **err);

#ifdef __cplusplus
}
#endif

#endif /* UMI_EDITOR_H */
