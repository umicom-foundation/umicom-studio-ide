/* Umicom Studio IDE | Quality Centre v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/quality_centre.h"
#include <stdlib.h>
#include <string.h>
UmiStatus umi_studio_quality_centre_create(UmiStudioServices *services,UmiStudioQualityCentre **out_centre)
{
    UmiStudioQualityCentre *centre;
    UmiStatus status;
    if (services == NULL || out_centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_centre = NULL;
    centre = (UmiStudioQualityCentre *)calloc(1U,sizeof(*centre));
    if (centre == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    centre->services = services;
    umi_codeguard_quality_session_init(&centre->session);
    status = umi_codeguard_quality_copy(centre->active_view,sizeof(centre->active_view),"overview");
    if (status != UMI_STATUS_OK) { free(centre); return status; }
    centre->revision = 1U;
    *out_centre = centre;
    return UMI_STATUS_OK;
}
void umi_studio_quality_centre_destroy(UmiStudioQualityCentre *centre) { free(centre); }
UmiStatus umi_studio_quality_centre_activate(UmiStudioQualityCentre *centre,const char *view_id)
{
    UmiStatus status;
    if (centre == NULL || view_id == NULL || view_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_codeguard_quality_copy(centre->active_view,sizeof(centre->active_view),view_id);
    if (status == UMI_STATUS_OK) centre->revision += 1U;
    return status;
}
UmiStatus umi_studio_quality_centre_snapshot(const UmiStudioQualityCentre *centre,UmiStudioQualitySnapshot *out_snapshot)
{
    if (centre == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot,0,sizeof(*out_snapshot));
    out_snapshot->evidence = centre->session.evidence.count;
    out_snapshot->failures = centre->session.evaluation.failures;
    out_snapshot->warnings = centre->session.evaluation.warnings;
    out_snapshot->duplicate_candidates = centre->session.duplicates.count;
    out_snapshot->api_changes = centre->session.api.count;
    out_snapshot->remediation_actions = centre->session.remediation.count;
    out_snapshot->decision = centre->session.evaluation.decision;
    out_snapshot->revision = centre->revision;
    return umi_codeguard_quality_copy(out_snapshot->active_view,sizeof(out_snapshot->active_view),centre->active_view);
}
