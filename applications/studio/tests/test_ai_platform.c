/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_ai_platform.c
 *
 * PURPOSE:
 *   Validate Studio's Framework-backed AI and Helix integration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused test verifies one product adapter while keeping the provider-neutral implementation inside Framework.
 */



#include <assert.h>

#include "umicom/studio/ai_platform.h"

int main(void)
{
    UmiStudioAiPlatform *platform = NULL;
    UmiAiAuthorEngineServiceSnapshot snapshot;
    assert(umi_studio_ai_platform_create(&platform) == UMI_STATUS_OK);
    assert(platform != NULL);
    assert(umi_studio_ai_platform_runtime(platform) != NULL);
    assert(umi_studio_ai_platform_helix(platform) != NULL);
    assert(umi_studio_ai_platform_authorengine(platform) != NULL);
    assert(umi_studio_ai_platform_snapshot(platform, &snapshot)
        == UMI_STATUS_OK);
    assert(snapshot.runtimes == 3U);
    assert(snapshot.context_sources == 3U);
    assert(snapshot.sessions == 1U);
    umi_studio_ai_platform_destroy(platform);
    return 0;
}
