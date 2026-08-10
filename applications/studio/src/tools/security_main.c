/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/security_main.c
 *
 * PURPOSE:
 *   Inspect Studio identities, roles, sessions, trust and security-event evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/security.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioSecurityReport report;
    UmiStatus status = umi_studio_bootstrap_create(&bootstrap);
    int exit_code = 0;

    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Security command failed: %s\n", umi_status_text(status));
        return 1;
    }
    if (argc >= 3 && strcmp(argv[1], "trust") == 0) {
        status = umi_studio_security_set_workspace_trust(
            umi_studio_bootstrap_services(bootstrap),
            argv[2],
            UMI_WORKSPACE_TRUSTED);
    } else if (argc >= 3 && strcmp(argv[1], "check") == 0) {
        UmiAuthorisationDecision decision;
        status = umi_studio_security_authorise(
            umi_studio_bootstrap_services(bootstrap),
            "studio.local-user",
            "studio.developer",
            argv[2],
            argc >= 4 ? argv[3] : "studio",
            &decision);
        if (status == UMI_STATUS_OK) {
            (void)printf("Allowed: %s\nReason: %s\n",
                         decision.allowed ? "yes" : "no",
                         decision.reason);
        }
    } else {
        status = umi_studio_security_report(
            umi_studio_bootstrap_services(bootstrap),
            &report);
        if (status == UMI_STATUS_OK) {
            (void)printf("Identities: %zu\nRoles: %zu\nSessions: %zu\n"
                         "Trusted workspaces: %zu\nSecurity events: %zu\n"
                         "Denied events: %zu\nPolicy rules: %zu\n",
                         report.identities, report.roles, report.sessions,
                         report.trusted_workspaces, report.events,
                         report.denied_events, report.policy_rules);
        }
    }
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Security command failed: %s\n", umi_status_text(status));
        exit_code = 1;
    }
    umi_studio_bootstrap_destroy(bootstrap);
    return exit_code;
}
