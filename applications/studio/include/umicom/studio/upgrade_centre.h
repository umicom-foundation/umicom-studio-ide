/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/upgrade_centre.h
 *
 * PURPOSE:
 *   Prepare, approve and explain Studio upgrades and rollback points.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_UPGRADE_CENTRE_H
#define UMICOM_STUDIO_UPGRADE_CENTRE_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioUpgradeCentre {
    UmiUpgradePlan plan;
    char status[UMI_DELIVERY_TEXT_CAPACITY];
} UmiStudioUpgradeCentre;

UmiStatus umi_studio_upgrade_centre_prepare(
    UmiStudioUpgradeCentre *centre,
    const char *current_version,
    const char *target_version,
    uint64_t current_generation,
    uint64_t target_generation,
    int compatible);
UmiStatus umi_studio_upgrade_centre_approve(
    UmiStudioUpgradeCentre *centre,
    int backup_available);
int umi_studio_upgrade_centre_ready(
    const UmiStudioUpgradeCentre *centre);

#ifdef __cplusplus
}
#endif
#endif
