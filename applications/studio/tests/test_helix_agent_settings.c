/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_helix_agent_settings.c
 * PURPOSE: Verify typed Helix v2 governance defaults configure Studio.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio/helix_agent_settings.h"
int main(void)
{
    UmiSettings *settings = NULL;
    UmiStudioAiPlatformConfig config = umi_studio_ai_platform_config_default();
    assert(umi_studio_settings_create(&settings) == UMI_STATUS_OK);
    assert(umi_studio_helix_agent_settings_apply(settings, &config)
           == UMI_STATUS_OK);
    assert(config.helix_maximum_attempts == 3U);
    assert(config.helix_require_human_approval);
    assert(config.helix_allow_filesystem && config.helix_allow_build);
    assert(!config.helix_allow_source_control);
    umi_settings_destroy(settings);
    return 0;
}
