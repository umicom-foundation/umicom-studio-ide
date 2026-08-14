/* Umicom Studio IDE | Security approval administration v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_SECURITY_APPROVALS_H
#define UMICOM_STUDIO_SECURITY_APPROVALS_H
#include "umicom/studio/security_centre.h"
UmiStatus umi_studio_security_approval_request(UmiStudioSecurityCentre *centre,const UmiSecurityApproval *approval);
UmiStatus umi_studio_security_approval_decide(UmiStudioSecurityCentre *centre,const char *approval_id,const char *decided_by,UmiSecurityApprovalState state,const char *reason);
const UmiSecurityApproval *umi_studio_security_approval_find(const UmiStudioSecurityCentre *centre,const char *approval_id,uint64_t now_ns);
#endif
