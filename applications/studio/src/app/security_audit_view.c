/* Umicom Studio IDE | Security audit view v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/security_audit_view.h"

const UmiSecurityAuditEntry *umi_studio_security_audit_at(
    const UmiStudioSecurityCentre *centre,
    size_t index)
{
    return centre == NULL ? NULL : umi_security_audit_at(&centre->governance.audit,index);
}

size_t umi_studio_security_audit_denied(const UmiStudioSecurityCentre *centre)
{
    return centre == NULL ? 0U : umi_security_audit_denied_count(&centre->governance.audit);
}
