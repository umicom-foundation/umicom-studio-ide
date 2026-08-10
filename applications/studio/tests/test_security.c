/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_security.c
 *
 * PURPOSE:
 *   Verify Studio workspace trust, role authorisation and retained security
 *   evidence through Framework services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/studio/bootstrap.h"
#include "umicom/studio/security.h"

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioServices *services;
    UmiAuthorisationDecision decision;
    UmiStudioSecurityReport report;

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    services = umi_studio_bootstrap_services(bootstrap);
    assert(umi_studio_security_set_workspace_trust(
               services,
               ".",
               UMI_WORKSPACE_TRUSTED) == UMI_STATUS_OK);
    assert(umi_studio_security_authorise(
               services,
               "studio.local-user",
               "studio.developer",
               "studio.workspace.read",
               ".",
               &decision) == UMI_STATUS_OK);
    assert(decision.allowed);
    assert(umi_studio_security_report(services, &report) == UMI_STATUS_OK);
    assert(report.trusted_workspaces == 1U);
    assert(report.events >= 2U);
    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}
