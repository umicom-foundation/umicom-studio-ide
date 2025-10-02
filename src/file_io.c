/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/file_io.c
 * PURPOSE: Implementation of atomic save helpers
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "file_io.h"

gboolean umi_file_save_atomic(const char *path, const char *data, gssize len, GError **err){
  if(!path) return FALSE;
  gchar *tmp = g_strconcat(path, ".tmp", NULL);
  gboolean ok = g_file_set_contents(tmp, data, len, err);
  if(!ok){ g_free(tmp); return FALSE; }
  ok = g_rename(tmp, path) == 0;
  if(!ok && err) *err = g_error_new_literal(g_quark_from_string("umicom"), 1, "rename failed");
  g_free(tmp);
  return ok;
}
