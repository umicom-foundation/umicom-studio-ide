/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/core/recent_files.h
 * PURPOSE: Public API for the MRU (recent files) list, JSON-backed.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_RECENT_FILES_H
#define UMI_RECENT_FILES_H

#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Minimal public structure used by recent_files.c
 * 'items' holds char* (UTF-8 file paths). The array owns the strings.
 */
typedef struct UmiRecent {
  GPtrArray *items;   /* element type: char* (owned) */
  guint      max_items;
} UmiRecent;

/* Load MRU list from default JSON file (creates empty list if missing). */
UmiRecent *umi_recent_load(void);

/* Persist MRU list to disk. Returns TRUE on success. */
gboolean   umi_recent_save(const UmiRecent *r);

/* Insert 'path' at the front, removing dupes and trimming to max_items. */
void       umi_recent_add(UmiRecent *r, const char *path);

/* Free the MRU object and all owned entries. */
void       umi_recent_free(UmiRecent *r);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UMI_RECENT_FILES_H */
