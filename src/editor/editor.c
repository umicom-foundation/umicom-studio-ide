/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/editor/editor.c
 * PURPOSE: Implementation of the editor shell (editor area + status/output/problems)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#include <gtk/gtk.h>
#include <glib.h>

/* Project headers */
#include "editor.h"

/* Use the central status/output/problem modules instead of local stubs.
 * We conditionally include headers if they exist; otherwise we forward-declare
 * the minimal API so this file can still compile.
 */
#if __has_include("status_util.h")
  #include "status_util.h"
#else
  typedef struct _UmiStatus UmiStatus;
  GtkWidget *umi_status_widget(UmiStatus *s);
  UmiStatus *umi_status_new(void);
  void umi_status_set(UmiStatus *s, const char *text);
  void umi_status_flash(UmiStatus *s, const char *text, guint msec);
#endif

#if __has_include("output_pane.h")
  #include "output_pane.h"
#else
  typedef struct _UmiOutputPane UmiOutputPane;
  UmiOutputPane *umi_output_pane_new(void);
  GtkWidget *umi_output_pane_widget(UmiOutputPane *op);
  void umi_output_pane_append_line(UmiOutputPane *op, const char *line);
  void umi_output_pane_append_line_err(UmiOutputPane *op, const char *line);
#endif

#if __has_include("problem_list.h")
  #include "problem_list.h"
#else
  typedef struct _UmiProblemList UmiProblemList;
  UmiProblemList *umi_problem_list_new(void);
  GtkWidget *umi_problem_list_widget(UmiProblemList *pl);
#endif

/* Public (but simple) editor object.
 * Keep it lightweight: it just composes existing widgets.
 */
typedef struct _UmiEditor {
  GtkWidget      *root;      /* container we expose */
  GtkWidget      *paned;     /* vertical paned: top editor, bottom tabs */
  GtkTextBuffer  *buf;       /* simple text buffer for now */
  GtkWidget      *textview;  /* editing area placeholder */

  UmiStatus      *status;
  UmiOutputPane  *out;
  UmiProblemList *problems;
} UmiEditor;

/* Local helpers */
static GtkWidget* build_bottom_tabs(UmiEditor *ed){
  GtkWidget *tabs = gtk_notebook_new();
  gtk_notebook_set_tab_pos(GTK_NOTEBOOK(tabs), GTK_POS_BOTTOM);

  /* Output */
  GtkWidget *outw = umi_output_pane_widget(ed->out);
  GtkWidget *lab1 = gtk_label_new("Output");
  gtk_notebook_append_page(GTK_NOTEBOOK(tabs), outw, lab1);

  /* Problems */
  GtkWidget *probw = umi_problem_list_widget(ed->problems);
  GtkWidget *lab2 = gtk_label_new("Problems");
  gtk_notebook_append_page(GTK_NOTEBOOK(tabs), probw, lab2);

  /* Status (as a separate page makes it visible on Windows where statusbars are subtle) */
  GtkWidget *statw = umi_status_widget(ed->status);
  GtkWidget *lab3 = gtk_label_new("Status");
  gtk_notebook_append_page(GTK_NOTEBOOK(tabs), statw, lab3);

  return tabs;
}

/* API --------------------------------------------------------------------- */
UmiEditor* umi_editor_new(void){
  UmiEditor *ed = g_new0(UmiEditor, 1);

  ed->status   = umi_status_new();
  ed->out      = umi_output_pane_new();
  ed->problems = umi_problem_list_new();

  /* Top editor area: a simple GtkTextView for now */
  ed->buf      = gtk_text_buffer_new(NULL);
  ed->textview = gtk_text_view_new_with_buffer(ed->buf);
  gtk_text_view_set_monospace(GTK_TEXT_VIEW(ed->textview), TRUE);

  /* Root container: vertical paned (editor on top, tabs bottom) */
  ed->paned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
  gtk_widget_set_vexpand(ed->paned, TRUE);
  gtk_paned_set_start_child(GTK_PANED(ed->paned), ed->textview);

  GtkWidget *tabs = build_bottom_tabs(ed);
  gtk_widget_set_vexpand(tabs, TRUE);
  gtk_paned_set_end_child(GTK_PANED(ed->paned), tabs);

  /* Expose a simple box to allow docking in the main window */
  ed->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_append(GTK_BOX(ed->root), ed->paned);

  /* Friendly initial status */
  umi_status_set(ed->status, "Ready");

  return ed;
}

GtkWidget* umi_editor_widget(UmiEditor *ed){
  return ed ? ed->root : NULL;
}

/* Convenience helpers other modules might use */
void umi_editor_flash_status(UmiEditor *ed, const char *text, guint ms){
  if(!ed) return;
  umi_status_flash(ed->status, text ? text : "Done", ms ? ms : 1100);
}

void umi_editor_append_output(UmiEditor *ed, const char *line){
  if(!ed || !line) return;
  umi_output_pane_append_line(ed->out, line);
}

void umi_editor_append_error(UmiEditor *ed, const char *line){
  if(!ed || !line) return;
  umi_output_pane_append_line_err(ed->out, line);
}
/*---------------------------------------------------------------------------*/