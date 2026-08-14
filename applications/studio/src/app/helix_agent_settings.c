/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/helix_agent_settings.c
 * PURPOSE: Read validated Helix v2 settings into AI platform configuration.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/helix_agent_settings.h"

#include <stdint.h>

UmiStatus umi_studio_helix_agent_settings_apply(
    const UmiSettings *settings,
    UmiStudioAiPlatformConfig *config)
{
    int64_t attempts = 0;
    int require_approval = 0;
    int allow_filesystem = 0;
    int allow_build = 0;
    int allow_source_control = 0;
    UmiStatus status;
    if (settings == NULL || config == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_settings_get_integer(
        settings, UMI_STUDIO_SETTING_HELIX_MAXIMUM_ATTEMPTS, &attempts);
    if (status == UMI_STATUS_OK) status = umi_settings_get_boolean(
        settings, UMI_STUDIO_SETTING_HELIX_REQUIRE_APPROVAL, &require_approval);
    if (status == UMI_STATUS_OK) status = umi_settings_get_boolean(
        settings, UMI_STUDIO_SETTING_HELIX_ALLOW_FILESYSTEM, &allow_filesystem);
    if (status == UMI_STATUS_OK) status = umi_settings_get_boolean(
        settings, UMI_STUDIO_SETTING_HELIX_ALLOW_BUILD, &allow_build);
    if (status == UMI_STATUS_OK) status = umi_settings_get_boolean(
        settings, UMI_STUDIO_SETTING_HELIX_ALLOW_SOURCE_CONTROL,
        &allow_source_control);
    if (status != UMI_STATUS_OK) return status;
    if (attempts <= 0 || attempts > UINT32_MAX) return UMI_STATUS_INVALID_STATE;
    config->helix_maximum_attempts = (uint32_t)attempts;
    config->helix_require_human_approval = require_approval;
    config->helix_allow_filesystem = allow_filesystem;
    config->helix_allow_build = allow_build;
    config->helix_allow_source_control = allow_source_control;
    return UMI_STATUS_OK;
}
