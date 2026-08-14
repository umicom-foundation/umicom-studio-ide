/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/distribution_centre.c
 *
 * PURPOSE:
 *   Compose the complete Studio package, installer and release workflow.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/distribution_centre.h"
#include <string.h>

UmiStatus umi_studio_distribution_centre_refresh(
    UmiStudioDistributionCentre *centre)
{
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    centre->release.evidence = centre->supply_chain.evidence;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_distribution_centre_init(
    UmiStudioDistributionCentre *centre,
    const char *release_id,
    const char *version)
{
    const UmiUpdateChannel *channel;
    UmiStatus status;
    if (centre == NULL || release_id == NULL || version == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(centre, 0, sizeof(*centre));
    status = umi_studio_package_project_init(&centre->packages, version);
    if (status != UMI_STATUS_OK) return status;
    status = umi_studio_installer_centre_init(&centre->installers, version);
    if (status != UMI_STATUS_OK) return status;
    status = umi_studio_release_channels_init(&centre->channels);
    if (status != UMI_STATUS_OK) return status;
    umi_studio_supply_chain_init(&centre->supply_chain);
    channel = umi_studio_release_channels_current(&centre->channels);
    status = umi_product_release_init(
        &centre->release, release_id, UMI_RELEASE_STABLE,
        &centre->packages.windows, channel);
    if (status != UMI_STATUS_OK) return status;
    status = umi_product_release_add_installer(
        &centre->release, &centre->installers.windows);
    if (status != UMI_STATUS_OK) return status;
    status = umi_product_release_add_installer(
        &centre->release, &centre->installers.linux);
    if (status != UMI_STATUS_OK) return status;
    return umi_studio_distribution_centre_refresh(centre);
}

size_t umi_studio_distribution_centre_blockers(
    const UmiStudioDistributionCentre *centre)
{
    if (centre == NULL) return 1U;
    return umi_product_release_blocker_count(&centre->release);
}

int umi_studio_distribution_centre_ready(
    const UmiStudioDistributionCentre *centre)
{
    return centre != NULL && umi_product_release_ready(&centre->release);
}
