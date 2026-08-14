/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/knowledge_views.c
 * PURPOSE: Delegate Studio Knowledge Centre panes to Framework view models.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/knowledge_views.h"

UmiStatus umi_studio_knowledge_overview_view(
    UmiStudioAiPlatform *platform, const char *view_id,
    UmiUiViewModel **out_view)
{
    return umi_knowledge_ui_overview_view_create(
        view_id, umi_studio_ai_platform_knowledge(platform), out_view);
}

UmiStatus umi_studio_knowledge_collections_view(
    UmiStudioAiPlatform *platform, const char *view_id,
    UmiUiViewModel **out_view)
{
    return umi_knowledge_ui_collections_view_create(
        view_id, umi_studio_ai_platform_knowledge(platform), out_view);
}

UmiStatus umi_studio_knowledge_sources_view(
    UmiStudioAiPlatform *platform, const char *view_id,
    UmiUiViewModel **out_view)
{
    return umi_knowledge_ui_sources_view_create(
        view_id, umi_studio_ai_platform_knowledge(platform), out_view);
}

UmiStatus umi_studio_knowledge_search_view(
    UmiStudioAiPlatform *platform, const char *view_id,
    const char *query_text, UmiUiViewModel **out_view)
{
    UmiKnowledgeMatch matches[UMI_KNOWLEDGE_QUERY_RESULT_MAX];
    size_t count = 0U;
    UmiStatus status = umi_studio_knowledge_centre_search(
        platform, query_text, NULL, matches,
        UMI_KNOWLEDGE_QUERY_RESULT_MAX, &count);
    return status == UMI_STATUS_OK
        ? umi_knowledge_ui_results_view_create(
              view_id, umi_studio_ai_platform_knowledge(platform),
              matches, count, out_view)
        : status;
}

UmiStatus umi_studio_knowledge_source_view(
    const char *view_id, const UmiKnowledgeMatch *match,
    UmiUiViewModel **out_view)
{
    return umi_knowledge_ui_source_view_create(view_id, match, out_view);
}
