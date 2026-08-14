/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/knowledge_views.h
 * PURPOSE: Expose Studio view factories over Framework Knowledge Centre views.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Studio supplies view IDs while Framework supplies view data. */
#ifndef UMICOM_STUDIO_KNOWLEDGE_VIEWS_H
#define UMICOM_STUDIO_KNOWLEDGE_VIEWS_H

#include "umicom/studio/knowledge_centre.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_knowledge_overview_view(
    UmiStudioAiPlatform *platform, const char *view_id,
    UmiUiViewModel **out_view);
UmiStatus umi_studio_knowledge_collections_view(
    UmiStudioAiPlatform *platform, const char *view_id,
    UmiUiViewModel **out_view);
UmiStatus umi_studio_knowledge_sources_view(
    UmiStudioAiPlatform *platform, const char *view_id,
    UmiUiViewModel **out_view);
UmiStatus umi_studio_knowledge_search_view(
    UmiStudioAiPlatform *platform, const char *view_id,
    const char *query_text, UmiUiViewModel **out_view);
UmiStatus umi_studio_knowledge_source_view(
    const char *view_id, const UmiKnowledgeMatch *match,
    UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif
#endif
