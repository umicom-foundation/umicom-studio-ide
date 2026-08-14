/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/upgrade_centre_v2.c
 *
 * PURPOSE:
 *   Prepare, approve and explain Studio upgrades and rollback points.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/upgrade_centre_v2.h"
#include <stdio.h>
#include <string.h>

UmiStatus umi_studio_upgrade_centre_v2_prepare(
    UmiStudioUpgradeCentreV2 *centre,
    const char *current_version,
    const char *target_version,
    uint64_t current_generation,
    uint64_t target_generation,
    int compatible)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(centre, 0, sizeof(*centre));
    status = umi_upgrade_plan_v2_init(
        &centre->plan, current_version, target_version, current_generation,
        target_generation, compatible);
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(centre->status, sizeof(centre->status),
                   "Upgrade %s to %s is waiting for a backup.",
                   current_version, target_version);
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_upgrade_centre_v2_approve(
    UmiStudioUpgradeCentreV2 *centre,
    int backup_available)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_upgrade_plan_v2_authorise(&centre->plan, backup_available);
    if (status == UMI_STATUS_OK) {
        (void)snprintf(centre->status, sizeof(centre->status),
                       "Upgrade approved; rollback generation is %llu.",
                       (unsigned long long)
                           umi_upgrade_plan_v2_rollback_generation(&centre->plan));
    }
    return status;
}

int umi_studio_upgrade_centre_v2_ready(
    const UmiStudioUpgradeCentreV2 *centre)
{
    return centre != NULL &&
           umi_upgrade_plan_v2_validate(&centre->plan) == UMI_STATUS_OK;
}
