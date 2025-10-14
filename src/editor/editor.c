/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/editor/editor.c
 *
 * PURPOSE:
 *   Build a composite editor UI consisting of:
 *     - a Notebook with “Output” and “Problems” tabs (top area),
 *     - a placeholder bottom area for the future code-editor view,
 *     - small hooks so activating a problem logs/opens the file.
 *
 * DESIGN:
 *   - Strict GTK4 widgets; no deprecated API.
 *   - Uses Problems and Output modules only via their public headers.
 *   - Activation callback is wired through problem_list_new_with_cb(...).
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#include <glib.h>
#include <gtk/gtk.h>

#include "editor.h"          /* UmiEditor struct & API               */
#include "problem_list.h"    /* problem_list_* API                  */
#include "output_pane.h"     /* UmiOutputPane + widget accessor     */
#include "status.h"          /* shim → forwards to status_util.h     */

static void on_problem_activate(gpointer user, const char *file, int line, int col)
{
  UmiEditor *ed = (UmiEditor *)user;
  if (!ed || !ed->out || !file) return;

  char msg[512];
  g_snprintf(msg, sizeof(msg), "Open: %s:%d:%d", file, line, col);
  umi_output_pane_append_line_err(ed->out, msg);

  /* Optionally flash status if present. */
  /* if (ed->status) umi_status_flash(ed->status, msg, 1200); */
}

UmiEditor *umi_editor_new(void)
{
  UmiEditor *ed = g_new0(UmiEditor, 1);

  /* Minimal text buffer so file operations compile even without a view yet. */
  ed->buffer = gtk_text_buffer_new(NULL);

  /* Root vertical box */
  ed->root = GTK_WIDGET(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));

  /* Split: top notebook / bottom placeholder */
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

  /* Problems list — use callback-enabled constructor */
  ed->problems = problem_list_new_with_cb(on_problem_activate, ed);
  GtkWidget *prb_tab = gtk_label_new("Problems");
  gtk_notebook_append_page(GTK_NOTEBOOK(nb),
                           problem_list_widget(ed->problems),
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
  if (ed->root)   g_object_unref(ed->root); /* children destroyed with root */
  g_free(ed);
}
/*  END OF FILE */