/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/search.h
 *
 * PURPOSE:
 *   Expose Studio file-name and text-search use cases through the shared
 *   Framework workspace index without depending on a graphical search pane.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_SEARCH_H
#define UMICOM_STUDIO_SEARCH_H

#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_search_files(UmiStudioServices *services,
                                  const char *query,
                                  int case_sensitive,
                                  UmiFileIndexEntry *out_entries,
                                  size_t capacity,
                                  size_t *out_count);
UmiStatus umi_studio_search_text(UmiStudioServices *services,
                                 const UmiSearchRequest *request,
                                 UmiSearchMatchSink sink,
                                 void *user_data,
                                 UmiSearchStats *out_stats);

#ifdef __cplusplus
}
#endif

#endif
