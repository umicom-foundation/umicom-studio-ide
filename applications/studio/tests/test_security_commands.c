/* Umicom Studio IDE Tests | Security Centre commands v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "security_centre_fixture.h"

int main(void)
{
    UmiStudioSecurityCentre *centre = umi_test_security_centre();
    UmiStudioSecurityCentreSnapshot snapshot;
    UmiSecurityGovernanceDecision build;
    UmiSecurityGovernanceDecision plugin;
    assert(umi_studio_security_execute(
        centre,UMI_STUDIO_SECURITY_COMMAND_SEED,NULL) == UMI_STATUS_OK);
    assert(umi_studio_security_execute(
        centre,UMI_STUDIO_SECURITY_COMMAND_AUTHORISE_BUILD,&build) == UMI_STATUS_OK);
    assert(umi_studio_security_execute(
        centre,UMI_STUDIO_SECURITY_COMMAND_AUTHORISE_PLUGIN,&plugin) == UMI_STATUS_OK);
    assert(build.allowed && plugin.allowed);
    assert(umi_studio_security_centre_snapshot(centre,10U,&snapshot) == UMI_STATUS_OK);
    assert(snapshot.governance.audit_entries == 2U);
    assert(snapshot.governance.secret_references == 1U);
    umi_studio_security_centre_destroy(centre);
    return 0;
}
