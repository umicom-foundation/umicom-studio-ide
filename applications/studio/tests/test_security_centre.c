/* Umicom Studio IDE Tests | Security Centre v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "security_centre_fixture.h"

int main(void)
{
    UmiStudioSecurityCentre *centre = umi_test_security_centre();
    UmiStudioSecurityCentreSnapshot snapshot;
    assert(umi_studio_security_centre_activate(centre,"audit") == UMI_STATUS_OK);
    assert(umi_studio_security_centre_snapshot(centre,10U,&snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.active_view,"audit") == 0);
    assert(snapshot.governance.identities == 0U && snapshot.revision == 2U);
    umi_studio_security_centre_destroy(centre);
    return 0;
}
