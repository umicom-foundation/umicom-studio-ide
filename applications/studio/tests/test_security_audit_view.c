/* Umicom Studio IDE Tests | Security audit view v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "security_centre_fixture.h"
#include "umicom/studio/security_audit_view.h"

int main(void)
{
    UmiStudioSecurityCentre *centre = umi_test_seeded_security_centre();
    UmiSecurityAccessRequest request = {
        .identity_id = "unknown",
        .role_id = "developer",
        .permission = "project.build",
        .resource = "workspace://demo",
        .workspace_trust = UMI_WORKSPACE_TRUSTED,
        .environment = UMI_SECURITY_ENV_DEVELOPMENT,
        .now_ns = 10U,
        .correlation_id = 5601U
    };
    UmiSecurityGovernanceDecision decision;
    const UmiSecurityAuditEntry *entry;
    assert(umi_studio_security_centre_authorise(centre,&request,&decision) == UMI_STATUS_OK);
    assert(!decision.allowed && umi_studio_security_audit_denied(centre) == 1U);
    entry = umi_studio_security_audit_at(centre,0U);
    assert(entry != NULL && strcmp(entry->principal,"unknown") == 0);
    assert(strstr(entry->reason,"Identity") != NULL);
    umi_studio_security_centre_destroy(centre);
    return 0;
}
