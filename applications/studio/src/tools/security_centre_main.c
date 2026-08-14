/* Umicom Studio IDE | Native Security Centre command | Sammy Hegab | Umicom Foundation | MIT */
#include <stdio.h>
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/security_commands.h"

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioSecurityCentre *centre = NULL;
    UmiStudioSecurityCentreSnapshot snapshot;
    UmiSecurityGovernanceDecision decision;
    UmiStatus status = umi_studio_bootstrap_create(&bootstrap);
    if (status == UMI_STATUS_OK) {
        status = umi_studio_security_centre_create(
            umi_studio_bootstrap_services(bootstrap),&centre);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_security_execute(
            centre,UMI_STUDIO_SECURITY_COMMAND_SEED,NULL);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_security_execute(
            centre,UMI_STUDIO_SECURITY_COMMAND_AUTHORISE_BUILD,&decision);
    }
    if (status == UMI_STATUS_OK && !decision.allowed) {
        status = UMI_STATUS_PERMISSION_DENIED;
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_security_centre_snapshot(centre,10U,&snapshot);
    }
    if (status == UMI_STATUS_OK) {
        (void)printf("Security Centre: %zu identities, %zu approvals, %zu audit events\n",
                     snapshot.governance.identities,
                     snapshot.governance.approvals,
                     snapshot.governance.audit_entries);
    }
    umi_studio_security_centre_destroy(centre);
    umi_studio_bootstrap_destroy(bootstrap);
    return status == UMI_STATUS_OK ? 0 : 1;
}
