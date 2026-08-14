/* Umicom Studio IDE | Quality evidence composition | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/quality_evidence.h"
#include <string.h>
UmiStatus umi_studio_quality_record_check(UmiStudioQualityCentre *centre,const char *id,UmiCodeGuardEvidenceKind kind,bool passed,size_t observed,size_t allowed,const char *path,size_t line,const char *summary,const char *remediation)
{
    UmiCodeGuardEvidence evidence = {0};
    UmiStatus status;
    if (centre == NULL || id == NULL || path == NULL || summary == NULL || remediation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_codeguard_quality_copy(evidence.id,sizeof(evidence.id),id);
    if (status == UMI_STATUS_OK) status = umi_codeguard_quality_copy(evidence.path,sizeof(evidence.path),path);
    if (status == UMI_STATUS_OK) status = umi_codeguard_quality_copy(evidence.summary,sizeof(evidence.summary),summary);
    if (status == UMI_STATUS_OK) status = umi_codeguard_quality_copy(evidence.remediation,sizeof(evidence.remediation),remediation);
    if (status != UMI_STATUS_OK) return status;
    evidence.kind = kind; evidence.state = passed ? UMI_CODEGUARD_EVIDENCE_PASS : UMI_CODEGUARD_EVIDENCE_FAIL; evidence.observed = observed; evidence.allowed = allowed; evidence.line = line;
    status = umi_codeguard_quality_session_record(&centre->session,&evidence);
    if (status == UMI_STATUS_OK) centre->revision += 1U;
    return status;
}
UmiStatus umi_studio_quality_record_architecture_dependency(UmiStudioQualityCentre *centre,const char *source_path,const char *target_path)
{
    size_t before;
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    before = centre->session.evidence.count;
    status = umi_codeguard_architecture_check_dependency(&centre->session.architecture,source_path,target_path,&centre->session.evidence);
    if (status == UMI_STATUS_OK && centre->session.evidence.count != before) { centre->session.revision += 1U; centre->revision += 1U; }
    return status;
}
const UmiCodeGuardEvidence *umi_studio_quality_find_evidence(const UmiStudioQualityCentre *centre,const char *evidence_id)
{
    return centre == NULL ? NULL : umi_codeguard_evidence_find(&centre->session.evidence,evidence_id);
}
