/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/file_index.h
 * PURPOSE: Recursive file index (cache + refresh)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FILE_INDEX_H
#define UMICOM_FILE_INDEX_H

#include <glib.h>

typedef struct {
  gchar *root;
  GPtrArray *files; /* strdup'd absolute paths */
} UmiFileIndex;

UmiFileIndex *umi_index_build(const char *root);
void          umi_index_refresh(UmiFileIndex *idx);
void          umi_index_free(UmiFileIndex *idx);

#endif /* UMICOM_FILE_INDEX_H */
