/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/search.c
 *
 * PURPOSE:
 *   Adapt Studio file-name and workspace text searches to the shared Framework
 *   index and cancellable search implementation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/search.h"

UmiStatus umi_studio_search_files(UmiStudioServices *services,
                                  const char *query,
                                  int case_sensitive,
                                  UmiFileIndexEntry *out_entries,
                                  size_t capacity,
                                  size_t *out_count)
{
    if (services == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_file_index_find(umi_studio_services_file_index(services),
                               query,
                               case_sensitive,
                               out_entries,
                               capacity,
                               out_count);
}

UmiStatus umi_studio_search_text(UmiStudioServices *services,
                                 const UmiSearchRequest *request,
                                 UmiSearchMatchSink sink,
                                 void *user_data,
                                 UmiSearchStats *out_stats)
{
    if (services == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_search_file_index(umi_studio_services_file_index(services),
                                 request,
                                 sink,
                                 user_data,
                                 out_stats);
}
