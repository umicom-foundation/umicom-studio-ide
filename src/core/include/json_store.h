/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/core/json_store.h
 * PURPOSE: Simple JSON-backed key-value store (string->string) using JSON-GLib.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_JSON_STORE_H
#define UMI_JSON_STORE_H
#include <glib.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiJsonStore {
  char       *path;
  GHashTable *kv;
} UmiJsonStore;
UmiJsonStore *umi_json_store_load(const char *path);
const char *umi_json_get(UmiJsonStore *s, const char *key);
void        umi_json_set(UmiJsonStore *s, const char *key, const char *value);
gboolean    umi_json_save(UmiJsonStore *s, GError **err);
void        umi_json_free(UmiJsonStore *s);
#ifdef __cplusplus
}
#endif
#endif /* UMI_JSON_STORE_H */
