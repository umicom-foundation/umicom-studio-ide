/*----------------------------------------------------------------------------- 
 * Umicom Studio IDE
 * File: src/editor/editor.c
 * PURPOSE: Implementation of the editor widget
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-07 | MIT
 *---------------------------------------------------------------------------*/

#include "editor.h"
#include "status_util.h"     /* UmiStatus, umi_status_new, umi_status_widget, umi_status_set, umi_status_flash */
#include "output_pane.h"     /* UmiOutputPane, umi_output_pane_* */
#include "problem_list.h"    /* UmiProblemList, umi_problem_list_* */

static void goto_pos(GtkTextView *view, int line, int col){
  if(!view) return;
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

  /* Text view + scroller */
  ed->view    = GTK_TEXT_VIEW(gtk_text_view_new());
  ed->buffer  = gtk_text_view_get_buffer(ed->view);
  ed->scroller= gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(ed->scroller), GTK_WIDGET(ed->view));

  /* Status line */
  ed->status = umi_status_new();

  /* Bottom notebook (Output / Problems) */
  ed->bottom   = GTK_NOTEBOOK(gtk_notebook_new());
  gtk_notebook_set_tab_pos(ed->bottom, GTK_POS_BOTTOM);

  ed->out      = umi_output_pane_new();
  GtkWidget *outw = umi_output_pane_widget(ed->out);
  gtk_notebook_append_page(ed->bottom, outw, gtk_label_new("Output"));

  ed->problems = umi_problem_list_new(on_problem_activate, ed);
  GtkWidget *plw = umi_problem_list_widget(ed->problems);
  gtk_notebook_append_page(ed->bottom, plw, gtk_label_new("Problems"));

  /* Root container assembly */
  ed->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_append(GTK_BOX(ed->root), ed->scroller);
  gtk_box_append(GTK_BOX(ed->root), umi_status_widget(ed->status));
  gtk_box_append(GTK_BOX(ed->root), GTK_WIDGET(ed->bottom));

  return ed;
}

GtkWidget *umi_editor_widget(UmiEditor *ed){ return ed?ed->root:NULL; }

void umi_editor_jump(UmiEditor *ed, int line, int col){
  if(!ed) return;
  goto_pos(ed->view, line, col);
}

void umi_editor_open_file(UmiEditor *ed, const char *path, GError **err){
  if(!ed || !path) return;
  gchar *txt = NULL; gsize len = 0;
  GError *local = NULL;
  if(g_file_get_contents(path, &txt, &len, &local)){
    gtk_text_buffer_set_text(ed->buffer, txt, (gint)len);
    g_free(ed->current_file);
    ed->current_file = g_strdup(path);
    if(ed->status) umi_status_set(ed->status, path);
    g_free(txt);
  }else{
    if(err && !*err) *err = local; else if(local) g_error_free(local);
    if(ed->out && local) umi_output_pane_append_line_err(ed->out, local->message);
  }
}

void umi_editor_save(UmiEditor *ed, GError **err){
  if(!ed) return;
  if(!ed->current_file){ /* fall back to Save As when we don't have a filename */
    umi_editor_save_as(ed, err);
    return;
  }
  GtkTextIter a,b;
  gtk_text_buffer_get_bounds(ed->buffer, &a, &b);
  gchar *text = gtk_text_buffer_get_text(ed->buffer, &a, &b, FALSE);
  GError *local = NULL;
  if(!g_file_set_contents(ed->current_file, text, -1, &local)){
    if(err && !*err) *err = local; else if(local) g_error_free(local);
    if(ed->out && local) umi_output_pane_append_line_err(ed->out, local->message);
  }else{
    if(ed->status) umi_status_flash(ed->status, "Saved", 900);
  }
  g_free(text);
}

void umi_editor_save_as(UmiEditor *ed, GError **err){
  if(!ed) return;
  GtkFileDialog *fd = gtk_file_dialog_new();
  gtk_file_dialog_set_title(fd, "Save As");
  GError *local = NULL;
  GFile *file = gtk_file_dialog_save(fd, NULL, NULL, &local);
  if(!file){
    if(local){ if(err && !*err) *err = local; else g_error_free(local); }
    g_object_unref(fd);
    return;
  }
  char *path = g_file_get_path(file);
  g_object_unref(file);
  g_object_unref(fd);
  if(!path) return;

  g_free(ed->current_file);
  ed->current_file = g_strdup(path);
  g_free(path);
  umi_editor_save(ed, err);
}
