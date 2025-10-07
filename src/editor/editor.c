/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/editor/editor.c
 * PURPOSE: Editor composite widget (Problems + Output) and helpers
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#include <glib.h>
#include <gtk/gtk.h>

/* Project headers */
#include "editor.h"          /* UmiEditor struct & public API */
#include "problem_list.h"    /* UmiProblemList, UmiProblemActivateCb */
#include "output_pane.h"     /* UmiOutputPane, umi_output_pane_* */
#include "status_util.h"     /* UmiStatus (if your UmiEditor carries one) */

/*-----------------------------------------------------------------------------
 * Design notes
 * - The only functional change from the failing build is the callback
 *   signature for "problem activated". It must match problem_list.c, which
 *   invokes a callback of the form:
 *       on_activate(user, const char *file, int line, int col)
 *   Not "const UmiProblem *". This file implements that signature.
 * - We also include "problem_list.h" explicitly so the typedefs are visible.
 *---------------------------------------------------------------------------*/

/* Forward: local activate handler invoked by UmiProblemList */
static void on_problem_activate(gpointer user, const char *file, int line, int col);

/*-----------------------------------------------------------------------------
 * Public constructor
 *---------------------------------------------------------------------------*/
UmiEditor *umi_editor_new(void)
{
  UmiEditor *ed = g_new0(UmiEditor, 1);

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

  /* Problems list — pass the correct callback + user pointer */
  ed->problems = umi_problem_list_new(on_problem_activate, ed);
  GtkWidget *prb_tab = gtk_label_new("Problems");
  gtk_notebook_append_page(GTK_NOTEBOOK(nb),
                           umi_problem_list_widget(ed->problems),
                           prb_tab);

  /* Pack tabs into top area */
  gtk_paned_set_start_child(GTK_PANED(vpaned), nb);

  /* Bottom area placeholder (you can replace with an editor view later) */
  GtkWidget *placeholder = gtk_label_new("");
  gtk_paned_set_end_child(GTK_PANED(vpaned), placeholder);

  return ed;
}

/* Return the widget to pack in the main window */
GtkWidget *umi_editor_widget(UmiEditor *ed)
{
  return ed ? ed->root : NULL;
}

/*-----------------------------------------------------------------------------
 * File helpers (stubs for now; wire to real editor/view later)
 *---------------------------------------------------------------------------*/
gboolean umi_editor_open_file(UmiEditor *ed, const char *path, GError **err)
{
  (void)ed; (void)path; (void)err;
  /* TODO: load document into a view/buffer; report errors with GError */
  return TRUE;
}

gboolean umi_editor_save(UmiEditor *ed, GError **err)
{
  (void)ed; (void)err;
  /* TODO: save current buffer */
  return TRUE;
}

gboolean umi_editor_save_as(UmiEditor *ed, GError **err)
{
  (void)ed; (void)err;
  /* TODO: implement Save As dialog / new path */
  return TRUE;
}

/*-----------------------------------------------------------------------------
 * Callback from Problems pane
 *---------------------------------------------------------------------------*/
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

/*---------------------------------------------------------------------------
 * End of file
 *---------------------------------------------------------------------------*/
