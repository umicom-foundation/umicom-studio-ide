/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/recent_files.h
 * PURPOSE: MRU (most recently used) file list
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_RECENT_FILES_H
#define UMICOM_RECENT_FILES_H

#include <glib.h>

typedef struct {
  GPtrArray *items; /* array of strdup'd strings */
  guint max_items;
} UmiRecent;

UmiRecent *umi_recent_load(void);
gboolean   umi_recent_save(const UmiRecent *r);
void       umi_recent_add(UmiRecent *r, const char *path); /* moves to front */
void       umi_recent_free(UmiRecent *r);

#endif /* UMICOM_RECENT_FILES_H */
