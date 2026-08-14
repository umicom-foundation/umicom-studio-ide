/* Umicom Studio IDE | Security role and permission administration v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_SECURITY_ROLES_H
#define UMICOM_STUDIO_SECURITY_ROLES_H
#include "umicom/studio/security_centre.h"
UmiStatus umi_studio_security_permission_add(UmiStudioSecurityCentre *centre,const UmiSecurityPermissionDescriptor *permission);
UmiStatus umi_studio_security_role_define(UmiStudioSecurityCentre *centre,const char *role_id,const char *description);
UmiStatus umi_studio_security_role_grant(UmiStudioSecurityCentre *centre,const char *role_id,const char *permission);
UmiStatus umi_studio_security_role_assign(UmiStudioSecurityCentre *centre,const UmiSecurityRoleAssignment *assignment);
UmiStatus umi_studio_security_centre_authorise(UmiStudioSecurityCentre *centre,const UmiSecurityAccessRequest *request,UmiSecurityGovernanceDecision *out_decision);
#endif
