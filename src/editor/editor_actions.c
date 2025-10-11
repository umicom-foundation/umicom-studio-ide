/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/editor/editor_actions.c
 * PURPOSE: Minimal editor ops (placeholder; integrate GtkSourceView later)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *---------------------------------------------------------------------------*/
#include <gtk/gtk.h>
#include <glib.h>
#include "include/editor_actions.h"
#include "../util/log/include/status_util.h"

static GtkTextBuffer* ensure_buffer(UmiEditor *ed) { 
  if (!ed) return NULL;
  if (!ed->buffer) ed->buffer = gtk_text_buffer_new(NULL);
  return ed->buffer;
}

gboolean umi_editor_open_file(UmiEditor *ed, const char *path, GError **err){
  if(!ed || !path) return FALSE;
  gchar *txt=NULL; gsize len=0;
  if(!g_file_get_contents(path,&txt,&len,err)) return FALSE;
  GtkTextBuffer* buf = ensure_buffer(ed);
  if (!buf) { g_free(txt); return FALSE; }
  gtk_text_buffer_set_text(buf, txt, (gint)len);
  g_free(ed->current_file);
  ed->current_file = g_strdup(path);
  if(ed->status) umi_status_set(ed->status, path);
  g_free(txt);
  return TRUE;
}

gboolean umi_editor_save(UmiEditor *ed, GError **err){
  GtkTextBuffer* buf = ensure_buffer(ed);
  if(!ed || !ed->current_file || !buf) return FALSE;
  GtkTextIter s,e; gtk_text_buffer_get_bounds(buf, &s,&e);
  gchar *txt = gtk_text_buffer_get_text(buf, &s, &e, FALSE);
  gboolean ok = g_file_set_contents(ed->current_file, txt, -1, err);
  g_free(txt);
  if(ok && ed->status) umi_status_flash(ed->status, "Saved", 1200);
  return ok;
}

gboolean umi_editor_save_as_path(UmiEditor *ed, const char *path, GError **err){
  GtkTextBuffer* buf = ensure_buffer(ed);
  if(!ed || !path || !buf) return FALSE;
  GtkTextIter s,e; gtk_text_buffer_get_bounds(buf, &s,&e);
  gchar *txt = gtk_text_buffer_get_text(buf, &s, &e, FALSE);
  gboolean ok = g_file_set_contents(path, txt, -1, err);
  if(ok){ g_free(ed->current_file); ed->current_file = g_strdup(path); }
  g_free(txt);
  if(ok && ed->status) umi_status_flash(ed->status, "Saved As", 1200);
  return ok;
}

gboolean umi_editor_save_as(UmiEditor *ed, GError **err){
  (void)ed;
  if (err) *err = g_error_new_literal(G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
                                      "Save As dialog not implemented yet");
  return FALSE;
}

void umi_editor_new_file(UmiEditor *ed){
  GtkTextBuffer* buf = ensure_buffer(ed);
  if(!ed || !buf) return;
  gtk_text_buffer_set_text(buf, "", -1);
  g_clear_pointer(&ed->current_file, g_free);
  if(ed->status) umi_status_flash(ed->status, "New file", 900);
}
