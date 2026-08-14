/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/upgrade_centre.c
 *
 * PURPOSE:
 *   Prepare, approve and explain Studio upgrades and rollback points.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/upgrade_centre.h"
#include <stdio.h>
#include <string.h>

UmiStatus umi_studio_upgrade_centre_prepare(
    UmiStudioUpgradeCentre *centre,
    const char *current_version,
    const char *target_version,
    uint64_t current_generation,
    uint64_t target_generation,
    int compatible)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(centre, 0, sizeof(*centre));
    status = umi_upgrade_plan_init(
        &centre->plan, current_version, target_version, current_generation,
        target_generation, compatible);
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(centre->status, sizeof(centre->status),
                   "Upgrade %s to %s is waiting for a backup.",
                   current_version, target_version);
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_upgrade_centre_approve(
    UmiStudioUpgradeCentre *centre,
    int backup_available)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_upgrade_plan_authorise(&centre->plan, backup_available);
    if (status == UMI_STATUS_OK) {
        (void)snprintf(centre->status, sizeof(centre->status),
                       "Upgrade approved; rollback generation is %llu.",
                       (unsigned long long)
                           umi_upgrade_plan_rollback_generation(&centre->plan));
    }
    return status;
}

int umi_studio_upgrade_centre_ready(
    const UmiStudioUpgradeCentre *centre)
{
    return centre != NULL &&
           umi_upgrade_plan_validate(&centre->plan) == UMI_STATUS_OK;
}
