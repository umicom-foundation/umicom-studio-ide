/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/security.h
 *
 * PURPOSE:
 *   Expose Studio security reports, workspace-trust decisions and permission
 *   checks through the shared Framework security context.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_SECURITY_H
#define UMICOM_STUDIO_SECURITY_H

#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioSecurityReport {
    size_t identities;
    size_t roles;
    size_t sessions;
    size_t trusted_workspaces;
    size_t events;
    size_t denied_events;
    size_t policy_rules;
} UmiStudioSecurityReport;

UmiStatus umi_studio_security_report(UmiStudioServices *services,
                                     UmiStudioSecurityReport *out_report);
UmiStatus umi_studio_security_set_workspace_trust(
    UmiStudioServices *services,
    const char *path,
    UmiWorkspaceTrustLevel level);
UmiStatus umi_studio_security_authorise(UmiStudioServices *services,
                                       const char *principal,
                                       const char *role_id,
                                       const char *permission,
                                       const char *resource,
                                       UmiAuthorisationDecision *out_decision);

#ifdef __cplusplus
}
#endif

#endif
