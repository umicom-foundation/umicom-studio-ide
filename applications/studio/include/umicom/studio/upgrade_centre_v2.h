/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/upgrade_centre_v2.h
 *
 * PURPOSE:
 *   Prepare, approve and explain Studio upgrades and rollback points.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_UPGRADE_CENTRE_V2_H
#define UMICOM_STUDIO_UPGRADE_CENTRE_V2_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioUpgradeCentreV2 {
    UmiUpgradePlanV2 plan;
    char status[UMI_DELIVERY_TEXT_CAPACITY];
} UmiStudioUpgradeCentreV2;

UmiStatus umi_studio_upgrade_centre_v2_prepare(
    UmiStudioUpgradeCentreV2 *centre,
    const char *current_version,
    const char *target_version,
    uint64_t current_generation,
    uint64_t target_generation,
    int compatible);
UmiStatus umi_studio_upgrade_centre_v2_approve(
    UmiStudioUpgradeCentreV2 *centre,
    int backup_available);
int umi_studio_upgrade_centre_v2_ready(
    const UmiStudioUpgradeCentreV2 *centre);

#ifdef __cplusplus
}
#endif
#endif
