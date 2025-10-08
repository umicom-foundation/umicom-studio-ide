/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/editor_actions.c
 * PURPOSE: Minimal editor ops (placeholder impl; integrate with GtkSourceView later)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "editor_actions.h"
#include "status_util.h"
#include <stdio.h>

gboolean umi_editor_open_file(UmiEditor *ed, const char *path, GError **err){
  if(!ed || !path) return FALSE;
  gchar *txt=NULL; gsize len=0;
  if(!g_file_get_contents(path,&txt,&len,err)) return FALSE;
  gtk_text_buffer_set_text(GTK_TEXT_BUFFER(ed->buffer), txt, (gint)len);
  g_free(ed->current_file);
  ed->current_file = g_strdup(path);
  if(ed->status) umi_status_set(ed->status, path);
  g_free(txt);
  return TRUE;
}

gboolean umi_editor_save(UmiEditor *ed, GError **err){
  if(!ed || !ed->current_file) return FALSE;
  GtkTextIter s,e; gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(ed->buffer), &s,&e);
  gchar *txt = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(ed->buffer), &s, &e, FALSE);
  gboolean ok = g_file_set_contents(ed->current_file, txt, -1, err);
  g_free(txt);
  if(ok && ed->status) umi_status_flash(ed->status, "Saved", 1200);
  return ok;
}

gboolean umi_editor_save_as(UmiEditor *ed, const char *path, GError **err){
  if(!ed || !path) return FALSE;
  GtkTextIter s,e; gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(ed->buffer), &s,&e);
  gchar *txt = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(ed->buffer), &s, &e, FALSE);
  gboolean ok = g_file_set_contents(path, txt, -1, err);
  if(ok){ g_free(ed->current_file); ed->current_file = g_strdup(path); }
  g_free(txt);
  if(ok && ed->status) umi_status_flash(ed->status, "Saved As", 1200);
  return ok;
}

void umi_editor_new_file(UmiEditor *ed){
  if(!ed) return;
  gtk_text_buffer_set_text(GTK_TEXT_BUFFER(ed->buffer), "", -1);
  g_clear_pointer(&ed->current_file, g_free);
  if(ed->status) umi_status_flash(ed->status, "New file", 900);
}
