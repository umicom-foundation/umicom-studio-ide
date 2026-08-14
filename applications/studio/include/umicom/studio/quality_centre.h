/* Umicom Studio IDE | Quality Centre v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_QUALITY_CENTRE_H
#define UMICOM_STUDIO_QUALITY_CENTRE_H
#include "umicom/studio/services.h"
#include "umicom/codeguard/quality_session.h"
typedef struct UmiStudioQualityCentre {
    UmiStudioServices *services;
    UmiCodeGuardQualitySession session;
    char active_view[UMI_CODEGUARD_QUALITY_ID_CAPACITY];
    uint64_t revision;
} UmiStudioQualityCentre;
typedef struct UmiStudioQualitySnapshot {
    size_t evidence;
    size_t failures;
    size_t warnings;
    size_t duplicate_candidates;
    size_t api_changes;
    size_t remediation_actions;
    UmiCodeGuardQualityDecision decision;
    char active_view[UMI_CODEGUARD_QUALITY_ID_CAPACITY];
    uint64_t revision;
} UmiStudioQualitySnapshot;
UmiStatus umi_studio_quality_centre_create(UmiStudioServices *services,UmiStudioQualityCentre **out_centre);
void umi_studio_quality_centre_destroy(UmiStudioQualityCentre *centre);
UmiStatus umi_studio_quality_centre_activate(UmiStudioQualityCentre *centre,const char *view_id);
UmiStatus umi_studio_quality_centre_snapshot(const UmiStudioQualityCentre *centre,UmiStudioQualitySnapshot *out_snapshot);
#endif
