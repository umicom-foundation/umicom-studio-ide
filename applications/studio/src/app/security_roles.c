/* Umicom Studio IDE | Security role and permission administration v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/security_roles.h"
static UmiStatus revised(UmiStudioSecurityCentre *centre,UmiStatus status)
{
    if (status == UMI_STATUS_OK) { centre->governance.revision += 1U; centre->revision += 1U; }
    return status;
}
UmiStatus umi_studio_security_permission_add(UmiStudioSecurityCentre *centre,const UmiSecurityPermissionDescriptor *permission) { return centre == NULL ? UMI_STATUS_INVALID_ARGUMENT : revised(centre,umi_security_permission_catalogue_register(&centre->governance.permissions,permission)); }
UmiStatus umi_studio_security_role_define(UmiStudioSecurityCentre *centre,const char *role_id,const char *description) { return centre == NULL ? UMI_STATUS_INVALID_ARGUMENT : revised(centre,umi_security_role_policy_define(&centre->governance.roles,role_id,description)); }
UmiStatus umi_studio_security_role_grant(UmiStudioSecurityCentre *centre,const char *role_id,const char *permission) { return centre == NULL ? UMI_STATUS_INVALID_ARGUMENT : revised(centre,umi_security_role_policy_grant(&centre->governance.roles,role_id,permission)); }
UmiStatus umi_studio_security_role_assign(UmiStudioSecurityCentre *centre,const UmiSecurityRoleAssignment *assignment) { return centre == NULL ? UMI_STATUS_INVALID_ARGUMENT : revised(centre,umi_security_role_assignment_add(&centre->governance.assignments,assignment)); }
UmiStatus umi_studio_security_centre_authorise(UmiStudioSecurityCentre *centre,const UmiSecurityAccessRequest *request,UmiSecurityGovernanceDecision *out_decision) { return centre == NULL ? UMI_STATUS_INVALID_ARGUMENT : revised(centre,umi_security_governance_authorise(&centre->governance,request,out_decision)); }
