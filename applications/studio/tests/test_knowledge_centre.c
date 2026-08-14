/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_knowledge_centre.c
 * PURPOSE: Verify Studio composes ingestion and cited retrieval over Framework.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio/knowledge_centre.h"

int main(void)
{
    UmiStudioAiPlatformConfig config = umi_studio_ai_platform_config_default();
    UmiStudioAiPlatform *platform = NULL;
    UmiStudioKnowledgeSourceInput input = {0};
    UmiKnowledgeIngestionReport report;
    UmiKnowledgeMatch matches[8];
    UmiKnowledgeServiceSnapshot snapshot;
    size_t count = 0U;
    config.knowledge_source_capacity = 8U;
    config.knowledge_vector_capacity = 32U;
    assert(umi_studio_ai_platform_create_configured(&config, &platform)
           == UMI_STATUS_OK);
    (void)strcpy(input.collection_id, "code");
    (void)strcpy(input.source_id, "studio.ai-platform");
    (void)strcpy(input.title, "Studio AI Platform");
    (void)strcpy(input.uri, "file://applications/studio/src/app/ai_platform.c");
    (void)strcpy(input.language, "c");
    (void)strcpy(input.media_type, "text/x-c");
    input.kind = UMI_KNOWLEDGE_SOURCE_CODE;
    input.classification = UMI_AI_DATA_INTERNAL;
    assert(umi_studio_knowledge_centre_ingest_text(
        platform, &input,
        "AuthorEngine owns model orchestration. The Knowledge Centre owns "
        "offline retrieval and citations.", &report) == UMI_STATUS_OK);
    assert(umi_studio_knowledge_centre_search(
        platform, "offline retrieval citations", NULL,
        matches, 8U, &count) == UMI_STATUS_OK);
    assert(count > 0U);
    assert(strcmp(matches[0].citation.source_id, "studio.ai-platform") == 0);
    assert(umi_knowledge_service_snapshot(
        umi_studio_ai_platform_knowledge(platform), &snapshot) == UMI_STATUS_OK);
    assert(snapshot.collection_count == 3U && snapshot.source_count == 1U);
    umi_studio_ai_platform_destroy(platform);
    return 0;
}
