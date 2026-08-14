/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/distribution_centre.h
 *
 * PURPOSE:
 *   Compose the complete Studio package, installer and release workflow.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_DISTRIBUTION_CENTRE_H
#define UMICOM_STUDIO_DISTRIBUTION_CENTRE_H

#include "umicom/studio/installer_centre.h"
#include "umicom/studio/package_project.h"
#include "umicom/studio/release_channels.h"
#include "umicom/studio/supply_chain.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioDistributionCentre {
    UmiStudioPackageProject packages;
    UmiStudioInstallerCentre installers;
    UmiStudioReleaseChannels channels;
    UmiStudioSupplyChain supply_chain;
    UmiProductRelease release;
} UmiStudioDistributionCentre;

UmiStatus umi_studio_distribution_centre_init(
    UmiStudioDistributionCentre *centre,
    const char *release_id,
    const char *version);
UmiStatus umi_studio_distribution_centre_refresh(
    UmiStudioDistributionCentre *centre);
size_t umi_studio_distribution_centre_blockers(
    const UmiStudioDistributionCentre *centre);
int umi_studio_distribution_centre_ready(
    const UmiStudioDistributionCentre *centre);

#ifdef __cplusplus
}
#endif
#endif
