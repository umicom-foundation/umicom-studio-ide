/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/session.c
 * PURPOSE: JSON-backed session persistence
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "session.h"
#include <json-glib/json-glib.h>

static const char *SESSION_JSON = "config/session.json";

UmiSession *umi_session_load(void){
  UmiSession *s = g_new0(UmiSession,1);
  s->last_file = NULL;
  s->caret_line = 1;
  s->caret_col  = 1;
  gchar *txt=NULL; gsize len=0;
  if(g_file_get_contents(SESSION_JSON,&txt,&len,NULL)){
    JsonParser *p=json_parser_new();
    if(json_parser_load_from_data(p,txt,(gssize)len,NULL)){
      JsonObject *o=json_node_get_object(json_parser_get_root(p));
      if(json_object_has_member(o,"last_file")) s->last_file = g_strdup(json_object_get_string_member(o,"last_file"));
      if(json_object_has_member(o,"caret_line")) s->caret_line = json_object_get_int_member(o,"caret_line");
      if(json_object_has_member(o,"caret_col"))  s->caret_col  = json_object_get_int_member(o,"caret_col");
    }
    g_object_unref(p); g_free(txt);
  }
  return s;
}

gboolean umi_session_save(const UmiSession *s){
  if(!s) return FALSE;
  g_mkdir_with_parents("config",0755);
  JsonBuilder *b=json_builder_new(); json_builder_begin_object(b);
  json_builder_set_member_name(b,"last_file"); json_builder_add_string_value(b, s->last_file?s->last_file:"");
  json_builder_set_member_name(b,"caret_line"); json_builder_add_int_value(b, s->caret_line);
  json_builder_set_member_name(b,"caret_col");  json_builder_add_int_value(b, s->caret_col);
  json_builder_end_object(b);
  JsonGenerator *g=json_generator_new(); JsonNode *root=json_builder_get_root(b);
  json_generator_set_root(g,root); gchar *out=json_generator_to_data(g,NULL);
  gboolean ok=g_file_set_contents(SESSION_JSON,out,-1,NULL);
  g_free(out); json_node_free(root); g_object_unref(g); g_object_unref(b);
  return ok;
}

void umi_session_free(UmiSession *s){
  if(!s) return;
  g_free(s->last_file);
  g_free(s);
}
