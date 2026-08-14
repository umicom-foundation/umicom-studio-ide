/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/distribution_commands.h
 *
 * PURPOSE:
 *   Define commands and enablement for packaging, publishing and rollback.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_DISTRIBUTION_COMMANDS_H
#define UMICOM_STUDIO_DISTRIBUTION_COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiStudioDistributionCommand {
    UMI_STUDIO_DISTRIBUTION_BUILD_PACKAGE = 1,
    UMI_STUDIO_DISTRIBUTION_SCAN_DEPENDENCIES = 2,
    UMI_STUDIO_DISTRIBUTION_GENERATE_INSTALLER = 3,
    UMI_STUDIO_DISTRIBUTION_VERIFY_RELEASE = 4,
    UMI_STUDIO_DISTRIBUTION_PUBLISH_RELEASE = 5,
    UMI_STUDIO_DISTRIBUTION_CHECK_UPDATES = 6,
    UMI_STUDIO_DISTRIBUTION_ROLLBACK = 7
} UmiStudioDistributionCommand;

const char *umi_studio_distribution_command_id(
    UmiStudioDistributionCommand command);
int umi_studio_distribution_command_enabled(
    UmiStudioDistributionCommand command,
    int release_ready,
    int installed_generation_available);

#ifdef __cplusplus
}
#endif
#endif
