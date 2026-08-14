/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/distribution_commands_v2.c
 *
 * PURPOSE:
 *   Define commands and enablement for packaging, publishing and rollback.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/distribution_commands_v2.h"

const char *umi_studio_distribution_command_v2_id(
    UmiStudioDistributionCommandV2 command)
{
    switch (command) {
        case UMI_STUDIO_DISTRIBUTION_BUILD_PACKAGE_V2:
            return "distribution.build-package";
        case UMI_STUDIO_DISTRIBUTION_SCAN_DEPENDENCIES_V2:
            return "distribution.scan-dependencies";
        case UMI_STUDIO_DISTRIBUTION_GENERATE_INSTALLER_V2:
            return "distribution.generate-installer";
        case UMI_STUDIO_DISTRIBUTION_VERIFY_RELEASE_V2:
            return "distribution.verify-release";
        case UMI_STUDIO_DISTRIBUTION_PUBLISH_RELEASE_V2:
            return "distribution.publish-release";
        case UMI_STUDIO_DISTRIBUTION_CHECK_UPDATES_V2:
            return "distribution.check-updates";
        case UMI_STUDIO_DISTRIBUTION_ROLLBACK_V2:
            return "distribution.rollback";
        default:
            return "distribution.unknown";
    }
}

int umi_studio_distribution_command_v2_enabled(
    UmiStudioDistributionCommandV2 command,
    int release_ready,
    int installed_generation_available)
{
    switch (command) {
        case UMI_STUDIO_DISTRIBUTION_BUILD_PACKAGE_V2:
        case UMI_STUDIO_DISTRIBUTION_SCAN_DEPENDENCIES_V2:
        case UMI_STUDIO_DISTRIBUTION_GENERATE_INSTALLER_V2:
        case UMI_STUDIO_DISTRIBUTION_VERIFY_RELEASE_V2:
        case UMI_STUDIO_DISTRIBUTION_CHECK_UPDATES_V2:
            return 1;
        case UMI_STUDIO_DISTRIBUTION_PUBLISH_RELEASE_V2:
            return release_ready != 0;
        case UMI_STUDIO_DISTRIBUTION_ROLLBACK_V2:
            return installed_generation_available != 0;
        default:
            return 0;
    }
}
