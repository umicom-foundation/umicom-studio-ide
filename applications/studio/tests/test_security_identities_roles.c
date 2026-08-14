/* Umicom Studio IDE Tests | Security identities and roles v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "security_centre_fixture.h"
#include "umicom/studio/security_identities.h"

int main(void)
{
    UmiStudioSecurityCentre *centre = umi_test_seeded_security_centre();
    UmiSecurityGovernanceDecision decision;
    const UmiSecurityIdentityProfile *identity =
        umi_studio_security_identity_find(centre,"sammy");
    assert(identity != NULL && identity->enabled);
    assert(strcmp(identity->organisation,"Umicom Foundation") == 0);
    assert(umi_studio_security_execute(
        centre,UMI_STUDIO_SECURITY_COMMAND_AUTHORISE_BUILD,&decision) == UMI_STATUS_OK);
    assert(decision.allowed && !decision.approval_required);
    umi_studio_security_centre_destroy(centre);
    return 0;
}
