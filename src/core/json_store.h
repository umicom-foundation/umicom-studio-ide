/*
 * json_store.h
 * Lightweight JSON document store using JSON-GLib.
 *
 * This header intentionally exposes an opaque handle (UmiJsonStore)
 * and a small, pragmatic API:
 *   - create/free
 *   - set/get file path
 *   - load/save the JSON from/to disk
 *   - access/set the document root (JsonNode*)
 *
 * Notes:
 *  - All functions are NULL-safe where sensible.
 *  - All file operations use UTF-8 paths.
 *  - Errors are reported with GError in the caller’s domain.
 /*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/core/json_store.h
* PURPOSE: Implementation of JSON key-value store with json-glib
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
 */

#ifndef UMI_JSON_STORE_H
#define UMI_JSON_STORE_H

#include <glib.h>
#include <json-glib/json-glib.h>

G_BEGIN_DECLS

/* Opaque store */
typedef struct _UmiJsonStore UmiJsonStore;

/* Create a new store. If 'path' is non-NULL, it is stored but not loaded. */
UmiJsonStore *umi_json_store_new(const char *path);

/* Free the store and its owned resources. */
void          umi_json_store_free(UmiJsonStore *store);

/* Set or get the path associated with this store. Passing NULL clears it. */
void          umi_json_store_set_path(UmiJsonStore *store, const char *path);
const char   *umi_json_store_get_path(UmiJsonStore *store);

/* Load JSON from the current path into memory (overwriting any previous root).
 * Returns TRUE on success. On failure, FALSE and 'err' is set.
 */
gboolean      umi_json_store_load(UmiJsonStore *store, GError **err);

/* Save JSON from memory to the current path.
 * Returns TRUE on success. On failure, FALSE and 'err' is set.
 * If no path is set, returns FALSE and sets err.
 */
gboolean      umi_json_store_save(UmiJsonStore *store, GError **err);

/* Get/set the root node. The store owns the node; callers should not unref it.
 * If you set a new root, the store takes ownership (it will ref/sink as needed).
 */
JsonNode     *umi_json_store_get_root(UmiJsonStore *store);
void          umi_json_store_set_root(UmiJsonStore *store, JsonNode *root);

/* Convenience: ensure the root is a JsonObject and return it (creating an
 * empty object root if needed). Returns NULL on allocation failure.
 */
JsonObject   *umi_json_store_ensure_object_root(UmiJsonStore *store);

/* Convenience: ensure the root is a JsonArray and return it (creating an
 * empty array root if needed). Returns NULL on allocation failure.
 */
JsonArray    *umi_json_store_ensure_array_root(UmiJsonStore *store);

G_END_DECLS

#endif /* UMI_JSON_STORE_H */
