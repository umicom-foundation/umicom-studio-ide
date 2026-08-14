/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/knowledge_main.c
 * PURPOSE: Provide a native smoke command for the offline Knowledge Centre.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

#include "umicom/studio/knowledge_centre.h"

int main(int argc, char **argv)
{
    UmiStudioAiPlatform *platform = NULL;
    UmiStudioKnowledgeSourceInput input = {0};
    UmiKnowledgeIngestionReport report;
    UmiKnowledgeMatch matches[8];
    size_t count = 0U;
    const char *query = argc > 1 ? argv[1] : "native rag citations";
    UmiStatus status = umi_studio_ai_platform_create(&platform);
    if (status != UMI_STATUS_OK) return 1;
    (void)snprintf(input.collection_id, sizeof(input.collection_id),
                   "%s", "documents");
    (void)snprintf(input.source_id, sizeof(input.source_id),
                   "%s", "studio.knowledge.smoke");
    (void)snprintf(input.title, sizeof(input.title),
                   "%s", "Studio Knowledge Smoke Source");
    (void)snprintf(input.uri, sizeof(input.uri),
                   "%s", "memory://studio/knowledge-smoke");
    (void)snprintf(input.language, sizeof(input.language), "%s", "en");
    (void)snprintf(input.media_type, sizeof(input.media_type),
                   "%s", "text/plain");
    input.kind = UMI_KNOWLEDGE_SOURCE_DOCUMENT;
    input.classification = UMI_AI_DATA_INTERNAL;
    status = umi_studio_knowledge_centre_ingest_text(
        platform, &input,
        "Umicom Native RAG provides offline retrieval, citations and exact "
        "source provenance without replacing AuthorEngine.", &report);
    if (status == UMI_STATUS_OK) {
        status = umi_studio_knowledge_centre_search(
            platform, query, NULL, matches, 8U, &count);
    }
    if (status == UMI_STATUS_OK) {
        (void)printf("Knowledge Centre: %zu result(s), %zu indexed chunk(s)\n",
                     count, report.chunks_created);
        if (count > 0U) {
            (void)printf("Best source: %s (%.4f)\n",
                         matches[0].citation.title, matches[0].score);
        }
    }
    umi_studio_ai_platform_destroy(platform);
    return status == UMI_STATUS_OK ? 0 : 1;
}
