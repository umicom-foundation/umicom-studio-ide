/* Umicom Studio IDE | API compatibility inspection | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/quality_compatibility.h"
UmiStatus umi_studio_quality_compare_api(UmiStudioQualityCentre *centre,const UmiCodeGuardApiBaseline *expected,const UmiCodeGuardApiBaseline *current)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_codeguard_api_baseline_compare(expected,current,&centre->session.api);
    if (status == UMI_STATUS_OK) { centre->session.revision += 1U; centre->revision += 1U; }
    return status;
}
const UmiCodeGuardApiChange *umi_studio_quality_api_change_at(const UmiStudioQualityCentre *centre,size_t index)
{
    return centre != NULL && index < centre->session.api.count ? &centre->session.api.changes[index] : NULL;
}
