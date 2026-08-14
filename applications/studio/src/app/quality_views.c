/* Umicom Studio IDE | Quality Centre views v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/quality_views.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
const char *umi_studio_quality_view_id(UmiStudioQualityViewKind kind)
{
    switch (kind) {
        case UMI_STUDIO_QUALITY_VIEW_OVERVIEW: return "overview";
        case UMI_STUDIO_QUALITY_VIEW_EVIDENCE: return "evidence";
        case UMI_STUDIO_QUALITY_VIEW_ARCHITECTURE: return "architecture";
        case UMI_STUDIO_QUALITY_VIEW_DUPLICATES: return "duplicates";
        case UMI_STUDIO_QUALITY_VIEW_COMPATIBILITY: return "compatibility";
        case UMI_STUDIO_QUALITY_VIEW_GATES: return "gates";
        case UMI_STUDIO_QUALITY_VIEW_REMEDIATION: return "remediation";
        default: return "unknown";
    }
}
static const char *title(UmiStudioQualityViewKind kind)
{
    switch (kind) {
        case UMI_STUDIO_QUALITY_VIEW_OVERVIEW: return "Quality Centre";
        case UMI_STUDIO_QUALITY_VIEW_EVIDENCE: return "Quality Evidence";
        case UMI_STUDIO_QUALITY_VIEW_ARCHITECTURE: return "Architecture Boundaries";
        case UMI_STUDIO_QUALITY_VIEW_DUPLICATES: return "Duplicate Review";
        case UMI_STUDIO_QUALITY_VIEW_COMPATIBILITY: return "ABI and API Compatibility";
        case UMI_STUDIO_QUALITY_VIEW_GATES: return "Combined Quality Gates";
        case UMI_STUDIO_QUALITY_VIEW_REMEDIATION: return "Remediation Guidance";
        default: return "Quality";
    }
}
UmiStatus umi_studio_quality_view_build(const UmiStudioQualityCentre *centre,UmiStudioQualityViewKind kind,UmiStudioQualityView *out_view)
{
    UmiStudioQualitySnapshot snapshot;
    int length;
    UmiStatus status;
    if (centre == NULL || out_view == NULL || kind < UMI_STUDIO_QUALITY_VIEW_OVERVIEW || kind > UMI_STUDIO_QUALITY_VIEW_REMEDIATION) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_studio_quality_centre_snapshot(centre,&snapshot);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_view,0,sizeof(*out_view)); out_view->kind = kind;
    (void)umi_codeguard_quality_copy(out_view->id,sizeof(out_view->id),umi_studio_quality_view_id(kind));
    (void)umi_codeguard_quality_copy(out_view->title,sizeof(out_view->title),title(kind));
    switch (kind) {
        case UMI_STUDIO_QUALITY_VIEW_EVIDENCE: out_view->item_count = snapshot.evidence; break;
        case UMI_STUDIO_QUALITY_VIEW_ARCHITECTURE: out_view->item_count = umi_codeguard_evidence_count_kind(&centre->session.evidence,UMI_CODEGUARD_EVIDENCE_ARCHITECTURE); break;
        case UMI_STUDIO_QUALITY_VIEW_DUPLICATES: out_view->item_count = snapshot.duplicate_candidates; break;
        case UMI_STUDIO_QUALITY_VIEW_COMPATIBILITY: out_view->item_count = snapshot.api_changes; break;
        case UMI_STUDIO_QUALITY_VIEW_GATES: out_view->item_count = snapshot.failures + snapshot.warnings; break;
        case UMI_STUDIO_QUALITY_VIEW_REMEDIATION: out_view->item_count = snapshot.remediation_actions; break;
        default: out_view->item_count = snapshot.evidence + snapshot.duplicate_candidates + snapshot.api_changes; break;
    }
    length = snprintf(out_view->summary,sizeof(out_view->summary),"%s: %zu item(s), decision %s, revision %" PRIu64,out_view->title,out_view->item_count,umi_codeguard_quality_decision_text(snapshot.decision),snapshot.revision);
    if (length < 0 || (size_t)length >= sizeof(out_view->summary)) return UMI_STATUS_CAPACITY_EXCEEDED;
    out_view->revision = snapshot.revision;
    return UMI_STATUS_OK;
}
