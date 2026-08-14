/* Umicom Studio IDE | Security approval administration v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/security_approvals.h"
UmiStatus umi_studio_security_approval_request(UmiStudioSecurityCentre *centre,const UmiSecurityApproval *approval)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_security_approval_request(&centre->governance.approvals,approval);
    if (status == UMI_STATUS_OK) { centre->governance.revision += 1U; centre->revision += 1U; }
    return status;
}
UmiStatus umi_studio_security_approval_decide(UmiStudioSecurityCentre *centre,const char *approval_id,const char *decided_by,UmiSecurityApprovalState state,const char *reason)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_security_approval_decide(&centre->governance.approvals,approval_id,decided_by,state,reason);
    if (status == UMI_STATUS_OK) { centre->governance.revision += 1U; centre->revision += 1U; }
    return status;
}
const UmiSecurityApproval *umi_studio_security_approval_find(const UmiStudioSecurityCentre *centre,const char *approval_id,uint64_t now_ns) { return centre == NULL ? NULL : umi_security_approval_find(&centre->governance.approvals,approval_id,now_ns); }
