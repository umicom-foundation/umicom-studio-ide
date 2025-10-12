/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/fs/include/file_index.h
 * PURPOSE: Recursive file index (cache + refresh)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FILE_INDEX_H
#define UMICOM_FILE_INDEX_H

#include <glib.h>      /* GPtrArray, gchar, gboolean, etc. */

/* Simple index of files under a root. */
typedef struct {
  gchar     *root;     /* Canonicalized root path (never NULL after build). */
  GPtrArray *files;    /* Array of g_strdup'd absolute/canonicalized file paths. */
} UmiFileIndex;

/* Build a new index by scanning 'root' (skips hidden files/dirs, avoids symlink loops).
   Returns NULL on invalid root or allocation failure. */
UmiFileIndex *umi_index_build(const char *root);

/* Refresh an existing index in-place (re-scan, clear + rebuild). */
void          umi_index_refresh(UmiFileIndex *idx);

/* Free all resources owned by the index (safe on NULL). */
void          umi_index_free(UmiFileIndex *idx);

#endif /* UMICOM_FILE_INDEX_H */
