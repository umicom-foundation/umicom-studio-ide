/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/editor.c
 * PURPOSE: Implementation of the editor widget
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "editor.h"

static void goto_pos(GtkTextView *view, int line, int col){
  GtkTextBuffer *b = gtk_text_view_get_buffer(view);
  GtkTextIter it;
  gtk_text_buffer_get_iter_at_line_offset(b, &it, (line>0?line-1:0), (col>0?col-1:0));
  gtk_text_buffer_place_cursor(b, &it);
  gtk_text_view_scroll_to_iter(view, &it, 0.1, FALSE, 0, 0);
}

static void on_problem_activate(gpointer user, const char *file, int line, int col){
  UmiEditor *ed = (UmiEditor*)user;
  if(!ed) return;
  if(file && *file){
    GError *e=NULL;
    /* This uses editor_actions, but if not linked yet we can inline open. */
    gchar *txt=NULL; gsize len=0;
    if(g_file_get_contents(file,&txt,&len,&e)){
      gtk_text_buffer_set_text(ed->buffer, txt, (gint)len);
      g_free(ed->current_file);
      ed->current_file = g_strdup(file);
      if(ed->status) umi_status_set(ed->status, file);
      g_free(txt);
    }else if(e){
      if(ed->out) umi_output_pane_append_line_err(ed->out, e->message);
      g_error_free(e);
    }
  }
  goto_pos(ed->view, line, col);
}

UmiEditor *umi_editor_new(void){
  UmiEditor *ed = g_new0(UmiEditor,1);
  ed->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  /* Text view */
  ed->view = GTK_TEXT_VIEW(gtk_text_view_new());
  ed->buffer = gtk_text_view_get_buffer(ed->view);
  ed->scroller = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(ed->scroller), GTK_WIDGET(ed->view));
  gtk_box_append(GTK_BOX(ed->root), ed->scroller);

  /* Status line (simple) */
  ed->status = umi_status_new();
  gtk_box_append(GTK_BOX(ed->root), umi_status_widget(ed->status));

  /* Bottom notebook */
  ed->bottom = GTK_NOTEBOOK(gtk_notebook_new());
  gtk_notebook_set_tab_pos(ed->bottom, GTK_POS_BOTTOM);

  /* Output pane */
  ed->out = umi_output_pane_new();
  GtkWidget *outw = umi_output_pane_widget(ed->out);
  gtk_notebook_append_page(ed->bottom, outw, gtk_label_new("Output"));

  /* Problems list */
  ed->problems = umi_problem_list_new(on_problem_activate, ed);
  GtkWidget *plw = umi_problem_list_widget(ed->problems);
  gtk_notebook_append_page(ed->bottom, plw, gtk_label_new("Problems"));

  gtk_box_append(GTK_BOX(ed->root), GTK_WIDGET(ed->bottom));
  return ed;
}

GtkWidget *umi_editor_widget(UmiEditor *ed){ return ed?ed->root:NULL; }

void umi_editor_jump(UmiEditor *ed, int line, int col){
  if(!ed) return;
  goto_pos(ed->view, line, col);
}
