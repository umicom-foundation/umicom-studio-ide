/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/output_pane.c
 * PURPOSE: Implements a scrollable text console for build/run output
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#include "include/output_pane.h"
#include <string.h>

struct _UmiOutputPane {
  GtkWidget *root;
  GtkWidget *scroller;
  GtkWidget *view;
  GtkTextBuffer *buf;
};

static void append_text(GtkTextBuffer *b, const char *s){
  if(!b || !s) return;
  GtkTextIter end; gtk_text_buffer_get_end_iter(b, &end);
  gtk_text_buffer_insert(b, &end, s, -1);
}

UmiOutputPane* umi_output_pane_new(void){
  UmiOutputPane *p = g_new0(UmiOutputPane, 1);
  p->scroller = gtk_scrolled_window_new();
  p->buf = gtk_text_buffer_new(NULL);
  p->view = gtk_text_view_new_with_buffer(p->buf);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(p->view), FALSE);
  gtk_text_view_set_monospace(GTK_TEXT_VIEW(p->view), TRUE);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(p->scroller), p->view);
  p->root = p->scroller;
  return p;
}

GtkWidget* umi_output_pane_widget(UmiOutputPane *p){ return p ? p->root : NULL; }
void umi_output_pane_clear(UmiOutputPane *p){ if(!p) return; gtk_text_buffer_set_text(p->buf, "", -1); }
void umi_output_pane_append(UmiOutputPane *p, const char *text){ if(!p) return; append_text(p->buf, text); }
void umi_output_pane_append_line(UmiOutputPane *p, const char *text){ if(!p) return; append_text(p->buf, text); append_text(p->buf, "\n"); }
void umi_output_pane_append_line_err(UmiOutputPane *p, const char *text){ if(!p) return; append_text(p->buf, "[err] "); append_text(p->buf, text); append_text(p->buf, "\n"); }
