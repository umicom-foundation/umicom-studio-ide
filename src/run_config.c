/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/run_config.c
 * PURPOSE: Implementation of JSON-backed run configuration
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "run_config.h"
#include <json-glib/json-glib.h>

static const char *RUN_JSON = "config/run.json";

/* Defaults provide a simple echo command; platform-specific tweaks may apply */
static UmiRunConfig *defaults(void){
  UmiRunConfig *r = g_new0(UmiRunConfig,1);
  r->cwd = g_strdup(".");
#ifdef G_OS_WIN32
  r->argv_line = g_strdup("cmd /c echo Hello, Umicom");
#else
  r->argv_line = g_strdup("sh -lc 'echo Hello, Umicom'");
#endif
  r->env_multiline = g_strdup("");
  return r;
}

UmiRunConfig *umi_run_config_load(void){
  gchar *txt=NULL; gsize len=0;
  if(!g_file_get_contents(RUN_JSON,&txt,&len,NULL)) return defaults();
  JsonParser *p=json_parser_new();
  if(!json_parser_load_from_data(p,txt,(gssize)len,NULL)){ g_object_unref(p); g_free(txt); return defaults(); }
  JsonObject *o=json_node_get_object(json_parser_get_root(p));
  UmiRunConfig *r = defaults();
  if(json_object_has_member(o,"cwd")){ g_free(r->cwd); r->cwd = g_strdup(json_object_get_string_member(o,"cwd")); }
  if(json_object_has_member(o,"argv_line")){ g_free(r->argv_line); r->argv_line = g_strdup(json_object_get_string_member(o,"argv_line")); }
  if(json_object_has_member(o,"env_multiline")){ g_free(r->env_multiline); r->env_multiline = g_strdup(json_object_get_string_member(o,"env_multiline")); }
  g_object_unref(p); g_free(txt);
  return r;
}

gboolean umi_run_config_save(const UmiRunConfig *r){
  if(!r) return FALSE;
  g_mkdir_with_parents("config",0755);
  JsonBuilder *b=json_builder_new(); json_builder_begin_object(b);
  json_builder_set_member_name(b,"cwd"); json_builder_add_string_value(b, r->cwd?r->cwd:".");
  json_builder_set_member_name(b,"argv_line"); json_builder_add_string_value(b, r->argv_line?r->argv_line:"");
  json_builder_set_member_name(b,"env_multiline"); json_builder_add_string_value(b, r->env_multiline?r->env_multiline:"");
  json_builder_end_object(b);
  JsonGenerator *g=json_generator_new(); JsonNode *root=json_builder_get_root(b);
  json_generator_set_root(g,root); gchar *out=json_generator_to_data(g,NULL);
  gboolean ok=g_file_set_contents(RUN_JSON,out,-1,NULL);
  g_free(out); json_node_free(root); g_object_unref(g); g_object_unref(b);
  return ok;
}

void umi_run_config_free(UmiRunConfig *r){
  if(!r) return; g_free(r->cwd); g_free(r->argv_line); g_free(r->env_multiline); g_free(r);
}

GPtrArray *umi_run_config_to_argv(const UmiRunConfig *r){
  GPtrArray *a = g_ptr_array_new_with_free_func(g_free);
  if(!r || !r->argv_line){ g_ptr_array_add(a, g_strdup("true")); g_ptr_array_add(a,NULL); return a; }
  gchar **t = g_strsplit(r->argv_line, " ", -1);
  for(gchar **p=t; p && *p; ++p) if(**p) g_ptr_array_add(a, g_strdup(*p));
  g_strfreev(t);
  g_ptr_array_add(a,NULL);
  return a;
}

gchar **umi_run_config_to_envp(const UmiRunConfig *r){
  if(!r || !r->env_multiline || !*r->env_multiline) return NULL;
  GPtrArray *a = g_ptr_array_new_with_free_func(g_free);
  gchar **lines = g_strsplit(r->env_multiline, "\n", -1);
  for(gchar **p=lines; p && *p; ++p){
    gchar *s = g_strstrip(*p);
    if(*s && strchr(s,'=')) g_ptr_array_add(a, g_strdup(s));
  }
  g_strfreev(lines);
  g_ptr_array_add(a,NULL);
  return (gchar**)g_ptr_array_free(a, FALSE);
}
