/* Umicom Studio IDE Tests | Security approvals v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "security_centre_fixture.h"
#include "umicom/studio/security_approvals.h"

int main(void)
{
    UmiStudioSecurityCentre *centre = umi_test_seeded_security_centre();
    const UmiSecurityApproval *approval =
        umi_studio_security_approval_find(centre,"approve-plugin-install",10U);
    assert(approval != NULL && approval->state == UMI_SECURITY_APPROVAL_APPROVED);
    assert(umi_studio_security_approval_find(
        centre,"approve-plugin-install",1000U) == NULL);
    umi_studio_security_centre_destroy(centre);
    return 0;
}
