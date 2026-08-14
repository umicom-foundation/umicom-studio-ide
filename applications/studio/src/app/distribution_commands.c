/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/distribution_commands.c
 *
 * PURPOSE:
 *   Define commands and enablement for packaging, publishing and rollback.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/distribution_commands.h"

const char *umi_studio_distribution_command_id(
    UmiStudioDistributionCommand command)
{
    switch (command) {
        case UMI_STUDIO_DISTRIBUTION_BUILD_PACKAGE:
            return "distribution.build-package";
        case UMI_STUDIO_DISTRIBUTION_SCAN_DEPENDENCIES:
            return "distribution.scan-dependencies";
        case UMI_STUDIO_DISTRIBUTION_GENERATE_INSTALLER:
            return "distribution.generate-installer";
        case UMI_STUDIO_DISTRIBUTION_VERIFY_RELEASE:
            return "distribution.verify-release";
        case UMI_STUDIO_DISTRIBUTION_PUBLISH_RELEASE:
            return "distribution.publish-release";
        case UMI_STUDIO_DISTRIBUTION_CHECK_UPDATES:
            return "distribution.check-updates";
        case UMI_STUDIO_DISTRIBUTION_ROLLBACK:
            return "distribution.rollback";
        default:
            return "distribution.unknown";
    }
}

int umi_studio_distribution_command_enabled(
    UmiStudioDistributionCommand command,
    int release_ready,
    int installed_generation_available)
{
    switch (command) {
        case UMI_STUDIO_DISTRIBUTION_BUILD_PACKAGE:
        case UMI_STUDIO_DISTRIBUTION_SCAN_DEPENDENCIES:
        case UMI_STUDIO_DISTRIBUTION_GENERATE_INSTALLER:
        case UMI_STUDIO_DISTRIBUTION_VERIFY_RELEASE:
        case UMI_STUDIO_DISTRIBUTION_CHECK_UPDATES:
            return 1;
        case UMI_STUDIO_DISTRIBUTION_PUBLISH_RELEASE:
            return release_ready != 0;
        case UMI_STUDIO_DISTRIBUTION_ROLLBACK:
            return installed_generation_available != 0;
        default:
            return 0;
    }
}
