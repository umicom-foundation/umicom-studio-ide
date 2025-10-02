/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/problem_list.c
 * PURPOSE: Implementation: list model, view, and diagnostics parsing
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "problem_list.h"
#include "diagnostic_parsers.h"

struct _UmiProblemList {
  GtkWidget *root;      /* GtkScrolledWindow */
  GtkWidget *view;      /* GtkTreeView */
  GtkListStore *store;  /* (file, line, col, sev, message) */
  UmiProblemActivateCb on_act;
  gpointer user;
};

enum { COL_FILE, COL_LINE, COL_COL, COL_SEV, COL_MSG, N_COLS };

static void on_row_activated(GtkTreeView *tv, GtkTreePath *path, GtkTreeViewColumn *col, gpointer u){
  (void)col;
  UmiProblemList *pl = (UmiProblemList*)u;
  GtkTreeIter it;
  if(gtk_tree_model_get_iter(GTK_TREE_MODEL(pl->store), &it, path)){
    char *file=NULL; int line=1, c=1; int sev=0; char *msg=NULL;
    gtk_tree_model_get(GTK_TREE_MODEL(pl->store), &it,
                       COL_FILE, &file, COL_LINE, &line, COL_COL, &c, COL_SEV, &sev, COL_MSG, &msg, -1);
    if(pl->on_act) pl->on_act(pl->user, file?file:"", line, c);
    g_free(file); g_free(msg);
  }
}

static GtkWidget* make_view(UmiProblemList *pl){
  pl->store = gtk_list_store_new(N_COLS, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING);
  GtkWidget *tv = gtk_tree_view_new_with_model(GTK_TREE_MODEL(pl->store));
  gtk_tree_view_append_column(GTK_TREE_VIEW(tv),
    gtk_tree_view_column_new_with_attributes("File", gtk_cell_renderer_text_new(), "text", COL_FILE, NULL));
  gtk_tree_view_append_column(GTK_TREE_VIEW(tv),
    gtk_tree_view_column_new_with_attributes("Line", gtk_cell_renderer_text_new(), "text", COL_LINE, NULL));
  gtk_tree_view_append_column(GTK_TREE_VIEW(tv),
    gtk_tree_view_column_new_with_attributes("Col", gtk_cell_renderer_text_new(), "text", COL_COL, NULL));
  gtk_tree_view_append_column(GTK_TREE_VIEW(tv),
    gtk_tree_view_column_new_with_attributes("Type", gtk_cell_renderer_text_new(), "text", COL_SEV, NULL));
  gtk_tree_view_append_column(GTK_TREE_VIEW(tv),
    gtk_tree_view_column_new_with_attributes("Message", gtk_cell_renderer_text_new(), "text", COL_MSG, NULL));
  g_signal_connect(tv, "row-activated", G_CALLBACK(on_row_activated), pl);
  return tv;
}

UmiProblemList *umi_problem_list_new(UmiProblemActivateCb on_act, gpointer user){
  UmiProblemList *pl = g_new0(UmiProblemList,1);
  pl->on_act = on_act; pl->user = user;
  pl->root = gtk_scrolled_window_new();
  pl->view = make_view(pl);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(pl->root), pl->view);
  return pl;
}

GtkWidget *umi_problem_list_widget(UmiProblemList *pl){ return pl?pl->root:NULL; }

void umi_problem_list_clear(UmiProblemList *pl){
  if(!pl) return;
  gtk_list_store_clear(pl->store);
}

void umi_problem_list_add(UmiProblemList *pl, const char *file, int line, int col,
                          UmiDiagSeverity sev, const char *message){
  if(!pl) return;
  GtkTreeIter it;
  gtk_list_store_append(pl->store, &it);
  gtk_list_store_set(pl->store, &it,
                     COL_FILE, file?file:"",
                     COL_LINE, line,
                     COL_COL,  col,
                     COL_SEV,  (int)sev,
                     COL_MSG,  message?message:"",
                     -1);
}

gboolean umi_problem_parse_any(UmiProblemList *pl, const char *line){
  if(!pl || !line) return FALSE;
  UmiDiagParseResult r;
  if(umi_diag_parse_gcc_clang(line, &r) || umi_diag_parse_msvc(line, &r)){
    umi_problem_list_add(pl, r.file, r.line, r.col, r.sev, r.msg);
    g_free(r.file); g_free(r.msg);
    return TRUE;
  }
  return FALSE;
}
