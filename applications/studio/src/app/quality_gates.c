/* Umicom Studio IDE | Combined quality gates v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/quality_gates.h"
UmiStatus umi_studio_quality_set_policy(UmiStudioQualityCentre *centre,const UmiCodeGuardQualityPolicy *policy)
{
    if (centre == NULL || policy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    centre->session.policy = *policy;
    centre->session.revision += 1U;
    centre->revision += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_studio_quality_evaluate(UmiStudioQualityCentre *centre,UmiCodeGuardQualityEvaluation *out_evaluation)
{
    UmiStatus status;
    if (centre == NULL || out_evaluation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_codeguard_quality_session_evaluate(&centre->session);
    if (status == UMI_STATUS_OK) { *out_evaluation = centre->session.evaluation; centre->revision += 1U; }
    return status;
}
