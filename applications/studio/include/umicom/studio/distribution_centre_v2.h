/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/distribution_centre_v2.h
 *
 * PURPOSE:
 *   Compose the complete Studio package, installer and release workflow.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_DISTRIBUTION_CENTRE_V2_H
#define UMICOM_STUDIO_DISTRIBUTION_CENTRE_V2_H

#include "umicom/studio/installer_centre_v2.h"
#include "umicom/studio/package_project_v2.h"
#include "umicom/studio/release_channels_v2.h"
#include "umicom/studio/supply_chain_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioDistributionCentreV2 {
    UmiStudioPackageProjectV2 packages;
    UmiStudioInstallerCentreV2 installers;
    UmiStudioReleaseChannelsV2 channels;
    UmiStudioSupplyChainV2 supply_chain;
    UmiProductReleaseV2 release;
} UmiStudioDistributionCentreV2;

UmiStatus umi_studio_distribution_centre_v2_init(
    UmiStudioDistributionCentreV2 *centre,
    const char *release_id,
    const char *version);
UmiStatus umi_studio_distribution_centre_v2_refresh(
    UmiStudioDistributionCentreV2 *centre);
size_t umi_studio_distribution_centre_v2_blockers(
    const UmiStudioDistributionCentreV2 *centre);
int umi_studio_distribution_centre_v2_ready(
    const UmiStudioDistributionCentreV2 *centre);

#ifdef __cplusplus
}
#endif
#endif
