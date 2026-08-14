/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/distribution_commands_v2.h
 *
 * PURPOSE:
 *   Define commands and enablement for packaging, publishing and rollback.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_DISTRIBUTION_COMMANDS_V2_H
#define UMICOM_STUDIO_DISTRIBUTION_COMMANDS_V2_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiStudioDistributionCommandV2 {
    UMI_STUDIO_DISTRIBUTION_BUILD_PACKAGE_V2 = 1,
    UMI_STUDIO_DISTRIBUTION_SCAN_DEPENDENCIES_V2 = 2,
    UMI_STUDIO_DISTRIBUTION_GENERATE_INSTALLER_V2 = 3,
    UMI_STUDIO_DISTRIBUTION_VERIFY_RELEASE_V2 = 4,
    UMI_STUDIO_DISTRIBUTION_PUBLISH_RELEASE_V2 = 5,
    UMI_STUDIO_DISTRIBUTION_CHECK_UPDATES_V2 = 6,
    UMI_STUDIO_DISTRIBUTION_ROLLBACK_V2 = 7
} UmiStudioDistributionCommandV2;

const char *umi_studio_distribution_command_v2_id(
    UmiStudioDistributionCommandV2 command);
int umi_studio_distribution_command_v2_enabled(
    UmiStudioDistributionCommandV2 command,
    int release_ready,
    int installed_generation_available);

#ifdef __cplusplus
}
#endif
#endif
