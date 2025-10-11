/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/recent_files.c
 * PURPOSE: Implementation of MRU list (JSON-backed)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include <umicom/recent_files.h>
#include <json-glib/json-glib.h>

static const char *RECENT_JSON = "config/recent.json";

UmiRecent *umi_recent_load(void){
  UmiRecent *r = g_new0(UmiRecent,1);
  r->items = g_ptr_array_new_with_free_func(g_free);
  r->max_items = 20;
  gchar *txt=NULL; gsize len=0;
  if(g_file_get_contents(RECENT_JSON,&txt,&len,NULL)){
    JsonParser *p=json_parser_new();
    if(json_parser_load_from_data(p,txt,(gssize)len,NULL)){
      JsonArray *a = json_node_get_array(json_parser_get_root(p));
      if(a){
        guint n = json_array_get_length(a);
        for(guint i=0;i<n;i++){
          JsonNode *node = json_array_get_element(a,i);
          const char *s = json_node_get_string(node);
          if(s && *s) g_ptr_array_add(r->items, g_strdup(s));
        }
      }
    }
    g_object_unref(p); g_free(txt);
  }
  return r;
}

gboolean umi_recent_save(const UmiRecent *r){
  if(!r) return FALSE;
  g_mkdir_with_parents("config",0755);
  JsonBuilder *b=json_builder_new(); json_builder_begin_array(b);
  for(guint i=0;i<r->items->len;i++){
    json_builder_add_string_value(b, (const char*)r->items->pdata[i]);
  }
  json_builder_end_array(b);
  JsonGenerator *g=json_generator_new(); JsonNode *root=json_builder_get_root(b);
  json_generator_set_root(g,root); gchar *out=json_generator_to_data(g,NULL);
  gboolean ok=g_file_set_contents(RECENT_JSON,out,-1,NULL);
  g_free(out); json_node_free(root); g_object_unref(g); g_object_unref(b);
  return ok;
}

void umi_recent_add(UmiRecent *r, const char *path){
  if(!r || !path || !*path) return;
  for(guint i=0;i<r->items->len;i++){
    const char *s = (const char*)r->items->pdata[i];
    if(g_strcmp0(s,path)==0){
      g_ptr_array_remove_index(r->items, i);
      break;
    }
  }
  g_ptr_array_insert(r->items, 0, g_strdup(path));
  if(r->items->len > r->max_items){
    g_free((char*)r->items->pdata[r->items->len-1]);
    g_ptr_array_set_size(r->items, r->max_items);
  }
}

void umi_recent_free(UmiRecent *r){
  if(!r) return;
  g_ptr_array_free(r->items, TRUE);
  g_free(r);
}
