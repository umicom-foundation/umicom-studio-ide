/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/workspace.c
 * PURPOSE: Implementation of workspace root + persistence
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "workspace.h"
#include <json-glib/json-glib.h>

static const char *WS_JSON = "config/workspace.json";

UmiWorkspace *umi_workspace_new(UmiFileTree *tree){
  UmiWorkspace *w = g_new0(UmiWorkspace,1);
  w->tree = tree;
  return w;
}

static void persist(UmiWorkspace *ws){
  if(!ws) return;
  g_mkdir_with_parents("config",0755);
  JsonBuilder *b=json_builder_new(); json_builder_begin_object(b);
  json_builder_set_member_name(b,"root_dir"); json_builder_add_string_value(b, ws->root_dir?ws->root_dir:"");
  json_builder_end_object(b);
  JsonGenerator *g=json_generator_new(); JsonNode *root=json_builder_get_root(b);
  json_generator_set_root(g,root); gchar *out=json_generator_to_data(g,NULL);
  g_file_set_contents(WS_JSON,out,-1,NULL);
  g_free(out); json_node_free(root); g_object_unref(g); g_object_unref(b);
}

void umi_workspace_set_root(UmiWorkspace *ws, const char *dir){
  if(!ws) return;
  g_free(ws->root_dir);
  ws->root_dir = g_strdup(dir?dir:".");
  if(ws->tree) umi_file_tree_set_root(ws->tree, ws->root_dir);
  persist(ws);
}

void umi_workspace_restore(UmiWorkspace *ws){
  if(!ws) return;
  gchar *txt=NULL; gsize len=0;
  if(g_file_get_contents(WS_JSON,&txt,&len,NULL)){
    JsonParser *p=json_parser_new();
    if(json_parser_load_from_data(p,txt,(gssize)len,NULL)){
      JsonObject *o=json_node_get_object(json_parser_get_root(p));
      const char *root = json_object_has_member(o,"root_dir") ? json_object_get_string_member(o,"root_dir") : ".";
      umi_workspace_set_root(ws, root);
    }
    g_object_unref(p); g_free(txt);
  }
}

const char *umi_workspace_root(UmiWorkspace *ws){ return ws?ws->root_dir:NULL; }

void umi_workspace_free(UmiWorkspace *ws){
  if(!ws) return;
  g_free(ws->root_dir);
  g_free(ws);
}
