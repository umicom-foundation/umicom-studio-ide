/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_knowledge_settings.c
 * PURPOSE: Verify typed Knowledge Centre defaults configure the product service.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio/knowledge_settings.h"

int main(void)
{
    UmiSettings *settings = NULL;
    UmiStudioAiPlatformConfig config = umi_studio_ai_platform_config_default();
    assert(umi_studio_settings_create(&settings) == UMI_STATUS_OK);
    assert(umi_studio_knowledge_settings_apply(settings, &config)
           == UMI_STATUS_OK);
    assert(config.knowledge_source_capacity == 1024U);
    assert(config.knowledge_vector_capacity == 8192U);
    assert(config.knowledge_chunk_bytes == 1200U);
    assert(config.knowledge_overlap_bytes == 200U);
    assert(config.knowledge_result_limit == 12U);
    assert(config.knowledge_offline_only);
    assert(strcmp(config.knowledge_archive_path,
                  ".umicom/knowledge-centre.bin") == 0);
    umi_settings_destroy(settings);
    return 0;
}
