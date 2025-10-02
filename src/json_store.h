/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/json_store.h
 * PURPOSE: JSON key-value store backed by json-glib
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_JSON_STORE_H
#define UMICOM_JSON_STORE_H

#include <glib.h>

/* This module wraps tiny configuration storage in a very friendly way.
 * The idea is that new contributors can look at one function per concept:
 * - load:  read a JSON file into a GHashTable (strings only to keep it simple).
 * - save:  write the table back to disk, pretty-printed so humans can read it.
 * - get:   fetch a value by key (returns NULL if not set).
 * - set:   change a key, allocating copies so ownership is obvious.
 *
 * The file lives at 'config/settings.json' relative to the working directory.
 * In a bigger system you would pass in a path; we fix it here to keep the sample super clear.
 */
typedef struct {
  GHashTable *kv;   /* string -> string (dup'd) */
  gchar      *path; /* where we load/save */
} UmiJsonStore;

UmiJsonStore *umi_json_store_load(const char *path); /* returns empty store if file missing */
const char   *umi_json_get   (UmiJsonStore *s, const char *key);
void          umi_json_set   (UmiJsonStore *s, const char *key, const char *value);
gboolean      umi_json_save  (UmiJsonStore *s, GError **err);
void          umi_json_free  (UmiJsonStore *s);

#endif /* UMICOM_JSON_STORE_H */
