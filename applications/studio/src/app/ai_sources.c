/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/ai_sources.c
 *
 * PURPOSE:
 *   Create a Studio source record for AI citation and retrieval displays.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Source construction stays independent of the UI so chat, retrieval and future editor annotations can share attribution data.
 */

#include "umicom/studio/ai_sources.h"

#include <stdio.h>

UmiStatus umi_studio_ai_source(const char *source_id,
                               const char *title,
                               const char *uri,
                               UmiAiSource *source)
{
    int a;
    int b;
    int c;
    if (source_id == NULL || title == NULL || uri == NULL || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_ai_source_init(source);
    a = snprintf(source->source_id, sizeof(source->source_id), "%s", source_id);
    b = snprintf(source->title, sizeof(source->title), "%s", title);
    c = snprintf(source->uri, sizeof(source->uri), "%s", uri);
    if (a < 0 || b < 0 || c < 0 ||
        (size_t)a >= sizeof(source->source_id) ||
        (size_t)b >= sizeof(source->title) ||
        (size_t)c >= sizeof(source->uri)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
