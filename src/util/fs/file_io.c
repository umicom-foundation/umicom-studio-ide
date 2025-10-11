/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/fs/file_io.c
 * PURPOSE: Implementation of atomic save helpers
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/file_io.h"        /* Our API */

#include <glib/gstdio.h>            /* g_rename (portable) */
#include <errno.h>                  /* errno for error reporting */
#include <string.h>                 /* strerror for human-readable errno text */

/* Atomic path write: "path.tmp" -> "path" via rename on success. */
gboolean umi_file_save_atomic(const char *path, const char *data, gssize len, GError **err){
  if(!path)                                        /* Validate required arg. */
    return FALSE;                                  /* No path => cannot save. */

  gchar *tmp = g_strconcat(path, ".tmp", NULL);    /* Create sibling temp path string. */

  /* Write file contents to temp path first. If len < 0, GLib will use strlen(data). */
  gboolean ok = g_file_set_contents(tmp, data, len, err); /* On failure, 'err' set by GLib. */
  if(!ok){                                         /* If write failed... */
    g_free(tmp);                                   /* ...free temp path and bail. */
    return FALSE;
  }

  /* Try to rename atomically into final destination. */
  if(g_rename(tmp, path) != 0){                    /* Non-zero -> rename failed. */
    int e = errno;                                 /* Snapshot errno ASAP. */
    if(err && !*err){                              /* If caller wants error and none set yet... */
      gchar *msg = g_strdup_printf("rename('%s'->'%s') failed: %s", tmp, path, g_strerror(e));
      *err = g_error_new_literal(g_quark_from_string("umi.io"), e ? e : 1, msg);
      g_free(msg);
    }
    /* Attempt to clean up dangling temp file. */
    (void)g_remove(tmp);                           /* Best-effort temp cleanup. */
    g_free(tmp);                                   /* Free temp path string. */
    return FALSE;                                  /* Report failure. */
  }

  g_free(tmp);                                     /* Free temp path on success as well. */
  return TRUE;                                     /* All good. */
}
