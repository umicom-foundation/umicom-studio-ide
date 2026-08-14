/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_knowledge_archive.c
 * PURPOSE: Verify Studio saves and restores its offline knowledge archive.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "umicom/studio/knowledge_centre.h"

static UmiStudioAiPlatform *create_platform(void)
{
    UmiStudioAiPlatformConfig config = umi_studio_ai_platform_config_default();
    UmiStudioAiPlatform *platform = NULL;
    config.knowledge_source_capacity = 8U;
    config.knowledge_vector_capacity = 32U;
    assert(umi_studio_ai_platform_create_configured(&config, &platform)
           == UMI_STATUS_OK);
    return platform;
}

int main(void)
{
    const char *path = "studio-knowledge-test.bin";
    UmiStudioAiPlatform *first = create_platform();
    UmiStudioAiPlatform *second = create_platform();
    UmiStudioKnowledgeSourceInput input = {0};
    UmiKnowledgeIngestionReport report;
    UmiKnowledgeServiceSnapshot snapshot;
    (void)strcpy(input.collection_id, "documents");
    (void)strcpy(input.source_id, "archive.source");
    (void)strcpy(input.title, "Archive Source");
    (void)strcpy(input.uri, "memory://archive");
    input.kind = UMI_KNOWLEDGE_SOURCE_DOCUMENT;
    input.classification = UMI_AI_DATA_INTERNAL;
    assert(umi_studio_knowledge_centre_ingest_text(
        first, &input, "Persistent offline retrieval.", &report)
        == UMI_STATUS_OK);
    assert(umi_studio_knowledge_centre_save(first, path) == UMI_STATUS_OK);
    assert(umi_studio_knowledge_centre_load(second, path) == UMI_STATUS_OK);
    assert(umi_knowledge_service_snapshot(
        umi_studio_ai_platform_knowledge(second), &snapshot) == UMI_STATUS_OK);
    assert(snapshot.source_count == 1U && snapshot.chunk_count == 1U);
    umi_studio_ai_platform_destroy(second);
    umi_studio_ai_platform_destroy(first);
    assert(remove(path) == 0);
    return 0;
}
