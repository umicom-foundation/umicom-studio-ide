/* Umicom Studio IDE | Security Centre v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_SECURITY_CENTRE_H
#define UMICOM_STUDIO_SECURITY_CENTRE_H
#include "umicom/studio/services.h"
#include "umicom/security/security_admin.h"
typedef struct UmiStudioSecurityCentre {
    UmiStudioServices *services;
    UmiSecurityGovernanceCentre governance;
    char active_view[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    uint64_t revision;
} UmiStudioSecurityCentre;
typedef struct UmiStudioSecurityCentreSnapshot {
    UmiSecurityGovernanceSnapshot governance;
    size_t enabled_identities;
    size_t pending_approvals;
    size_t secrets_due_rotation;
    char active_view[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    uint64_t revision;
} UmiStudioSecurityCentreSnapshot;
UmiStatus umi_studio_security_centre_create(UmiStudioServices *services,UmiStudioSecurityCentre **out_centre);
void umi_studio_security_centre_destroy(UmiStudioSecurityCentre *centre);
UmiStatus umi_studio_security_centre_activate(UmiStudioSecurityCentre *centre,const char *view_id);
UmiStatus umi_studio_security_centre_snapshot(const UmiStudioSecurityCentre *centre,uint64_t now_ns,UmiStudioSecurityCentreSnapshot *out_snapshot);
#endif
