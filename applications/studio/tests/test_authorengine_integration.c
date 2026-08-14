/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_authorengine_integration.c
 *
 * PURPOSE:
 *   Verify Studio composes configurable AuthorEngine runtimes without owning
 *   the provider-neutral orchestration implementation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/ai_platform.h"

int main(void)
{
    UmiStudioAiPlatformConfig config =
        umi_studio_ai_platform_config_default();
    UmiStudioAiPlatform *platform = NULL;
    UmiAiAuthorEngineServiceSnapshot snapshot;
    UmiAiContextPlan plan;
    UmiAiRuntimeCatalogue *catalogue;
    UmiAiRuntimeDescriptor remote;
    (void)strcpy(config.authorengine_executable, "C:\\Tools\\Umicom\\uaengine.exe");
    (void)strcpy(config.workspace, "C:\\Dev\\Umicom Workspace");
    config.context_tokens = 16384U;
    config.reserved_output_tokens = 2048U;
    config.allow_remote = 1;
    assert(umi_studio_ai_platform_create_configured(&config, &platform)
        == UMI_STATUS_OK);
    assert(umi_studio_ai_platform_authorengine(platform) != NULL);
    assert(umi_studio_ai_platform_snapshot(platform, &snapshot)
        == UMI_STATUS_OK);
    assert(snapshot.providers == 3U);
    assert(snapshot.runtimes == 3U);
    assert(snapshot.context_sources == 3U);
    assert(snapshot.sessions == 1U);
    assert(snapshot.remote_allowed);
    assert(strcmp(snapshot.executable, config.authorengine_executable) == 0);
    catalogue = umi_ai_authorengine_service_catalogue(
        umi_studio_ai_platform_authorengine(platform));
    assert(umi_ai_runtime_catalogue_find(
        catalogue, "authorengine.remote.chat", &remote) == UMI_STATUS_OK);
    assert(remote.configured);
    assert(umi_ai_authorengine_service_plan_context(
        umi_studio_ai_platform_authorengine(platform),
        "authorengine.local.chat", 0, &plan) == UMI_STATUS_OK);
    assert(plan.source_count == 2U);
    assert(plan.dropped_count == 0U);
    umi_studio_ai_platform_destroy(platform);
    return 0;
}
