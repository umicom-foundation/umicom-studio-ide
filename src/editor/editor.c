/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/editor/editor.c
 * PURPOSE: Editor composite widget (Problems + Output) and helpers
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *---------------------------------------------------------------------------*/
#include <glib.h>
#include <gtk/gtk.h>

/* Project headers */
#include <umicom/editor.h>          /* UmiEditor struct & public API */
#include "../panes/problems/include/problem_list.h"    /* UmiProblemList, UmiProblemActivateCb */
#include "../panes/output/include/output_pane.h"     /* UmiOutputPane, umi_output_pane_* */
#include "../util/log/include/status_util.h"     /* UmiStatus (optional) */

/* Forward: local activate handler invoked by UmiProblemList */
static void on_problem_activate(gpointer user, const char *file, int line, int col);

UmiEditor *umi_editor_new(void)
{
  UmiEditor *ed = g_new0(UmiEditor, 1);

  /* Minimal text buffer so open/save compile even without a view yet */
  ed->buffer = gtk_text_buffer_new(NULL);

  /* Root vertical box */
  ed->root = GTK_WIDGET(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));

  /* Split: top (tabs) / bottom (future editor view or preview) */
  GtkWidget *vpaned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
  gtk_box_append(GTK_BOX(ed->root), vpaned);

  /* Tabs for Output / Problems */
  GtkWidget *nb = gtk_notebook_new();

  /* Output pane */
  ed->out = umi_output_pane_new();
  GtkWidget *out_tab = gtk_label_new("Output");
  gtk_notebook_append_page(GTK_NOTEBOOK(nb),
                           umi_output_pane_widget(ed->out),
                           out_tab);

  /* Problems list â€” pass the correct callback + user pointer */
  ed->problems = umi_problem_list_new(on_problem_activate, ed);
  GtkWidget *prb_tab = gtk_label_new("Problems");
  gtk_notebook_append_page(GTK_NOTEBOOK(nb),
                           umi_problem_list_widget(ed->problems),
                           prb_tab);

  /* Pack tabs into top area */
  gtk_paned_set_start_child(GTK_PANED(vpaned), nb);

  /* Bottom area placeholder (replace with editor view later) */
  GtkWidget *placeholder = gtk_label_new("");
  gtk_paned_set_end_child(GTK_PANED(vpaned), placeholder);

  return ed;
}

GtkWidget *umi_editor_widget(UmiEditor *ed)
{
  return ed ? ed->root : NULL;
}

void umi_editor_free(UmiEditor *ed)
{
  if (!ed) return;
  g_clear_pointer(&ed->current_file, g_free);
  if (ed->buffer) g_object_unref(ed->buffer);
  /* out/probs are owned by GTK container; they will be finalized when 'root' is disposed */
  if (ed->root) g_object_unref(ed->root);
  g_free(ed);
}

static void on_problem_activate(gpointer user, const char *file, int line, int col)
{
  UmiEditor *ed = (UmiEditor *)user;
  if (!ed || !ed->out || !file) return;

  char msg[512];
  g_snprintf(msg, sizeof(msg), "Open: %s:%d:%d", file, line, col);
  umi_output_pane_append_line_err(ed->out, msg);

  /* If your UmiEditor carries a UmiStatus pointer, you can flash too:
     if (ed->status) umi_status_flash(ed->status, msg, 1200);
   */
}
