/* Umicom Studio IDE | Quality remediation guidance | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/quality_remediation.h"
UmiStatus umi_studio_quality_remediation_refresh(UmiStudioQualityCentre *centre)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_codeguard_remediation_plan_build(&centre->session.evidence,&centre->session.remediation);
    if (status == UMI_STATUS_OK) { centre->session.revision += 1U; centre->revision += 1U; }
    return status;
}
const UmiCodeGuardRemediationAction *umi_studio_quality_remediation_at(const UmiStudioQualityCentre *centre,size_t index)
{
    return centre == NULL ? NULL : umi_codeguard_remediation_plan_at(&centre->session.remediation,index);
}
