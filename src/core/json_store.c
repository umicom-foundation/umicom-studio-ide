/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/json_store.c
 * PURPOSE: Implementation of JSON key-value store with json-glib
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "json_store.h"
#include <json-glib/json-glib.h>

static GHashTable *new_table(void){
  return g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
}

UmiJsonStore *umi_json_store_load(const char *path){
  UmiJsonStore *s = g_new0(UmiJsonStore,1);
  s->path = g_strdup(path?path:"config/settings.json");
  s->kv = new_table();
  if(!g_file_test(s->path, G_FILE_TEST_EXISTS)) return s;
  GError *e=NULL;
  JsonParser *p = json_parser_new();
  if(json_parser_load_from_file(p, s->path, &e)){
    JsonNode *root = json_parser_get_root(p);
    if(JSON_NODE_HOLDS_OBJECT(root)){
      JsonObject *o = json_node_get_object(root);
      GList *keys = json_object_get_members(o);
      for(GList *l=keys;l;l=l->next){
        const char *k = (const char*)l->data;
        const char *v = json_object_get_string_member(o, k);
        if(k && v) g_hash_table_insert(s->kv, g_strdup(k), g_strdup(v));
      }
      g_list_free(keys);
    }
  }else if(e){ g_warning("json load: %s", e->message); g_error_free(e); }
  g_object_unref(p);
  return s;
}

const char *umi_json_get(UmiJsonStore *s, const char *key){
  if(!s || !key) return NULL;
  return (const char*)g_hash_table_lookup(s->kv, key);
}

void umi_json_set(UmiJsonStore *s, const char *key, const char *value){
  if(!s || !key) return;
  g_hash_table_insert(s->kv, g_strdup(key), g_strdup(value?value:""));
}

gboolean umi_json_save(UmiJsonStore *s, GError **err){
  if(!s) return FALSE;
  JsonBuilder *b = json_builder_new();
  json_builder_begin_object(b);
  GHashTableIter it; gpointer k,v; g_hash_table_iter_init(&it, s->kv);
  while(g_hash_table_iter_next(&it, &k, &v)){
    json_builder_set_member_name(b, (const char*)k);
    json_builder_add_string_value(b, (const char*)v);
  }
  json_builder_end_object(b);
  JsonGenerator *g = json_generator_new();
  JsonNode *root = json_builder_get_root(b);
  json_generator_set_root(g, root);
  json_generator_set_pretty(g, TRUE);
  gboolean ok = json_generator_to_file(g, s->path, err);
  g_object_unref(g);
  json_node_free(root);
  g_object_unref(b);
  return ok;
}

void umi_json_free(UmiJsonStore *s){
  if(!s) return;
  g_hash_table_destroy(s->kv);
  g_free(s->path);
  g_free(s);
}
