/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/security.c
 *
 * PURPOSE:
 *   Apply Studio defaults to Framework security services and retain auditable
 *   evidence for workspace trust and permission decisions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/security.h"

#include <stdio.h>
#include <string.h>

#include "umicom/studio/operations.h"

static UmiStudioOperations *studio_operations(UmiStudioServices *services)
{
    return umi_studio_services_operations(services);
}

UmiStatus umi_studio_security_report(UmiStudioServices *services,
                                     UmiStudioSecurityReport *out_report)
{
    UmiStudioOperations *operations;
    UmiSecurityContext *security;

    if (services == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    operations = studio_operations(services);
    security = umi_studio_operations_security(operations);
    if (security == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    out_report->identities = umi_identity_registry_count(
        umi_security_context_identities(security));
    out_report->roles = umi_role_registry_count(
        umi_security_context_roles(security));
    out_report->sessions = umi_session_registry_count(
        umi_security_context_sessions(security));
    out_report->trusted_workspaces = umi_workspace_trust_store_count(
        umi_security_context_workspace_trust(security));
    out_report->events = umi_security_event_log_count(
        umi_security_context_events(security));
    out_report->denied_events = umi_security_event_log_denied_count(
        umi_security_context_events(security));
    out_report->policy_rules = umi_policy_engine_count(
        umi_security_context_policy(security));
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_security_set_workspace_trust(
    UmiStudioServices *services,
    const char *path,
    UmiWorkspaceTrustLevel level)
{
    UmiStudioOperations *operations;
    UmiSecurityContext *security;
    UmiSecurityEvent event = {0};
    UmiStatus status;
    uint64_t now;

    if (services == NULL || path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    operations = studio_operations(services);
    security = umi_studio_operations_security(operations);
    if (security == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }
    now = umi_studio_operations_now(operations);
    status = umi_workspace_trust_store_set(
        umi_security_context_workspace_trust(security),
        path,
        level,
        "studio.local-user",
        now);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    event.timestamp_ns = now;
    (void)snprintf(event.principal,
                   sizeof(event.principal),
                   "%s",
                   "studio.local-user");
    (void)snprintf(event.action,
                   sizeof(event.action),
                   "%s",
                   "workspace.trust.set");
    (void)snprintf(event.resource,
                   sizeof(event.resource),
                   "%s",
                   path);
    event.allowed = 1;
    return umi_security_event_log_append(
        umi_security_context_events(security),
        &event);
}

UmiStatus umi_studio_security_authorise(UmiStudioServices *services,
                                       const char *principal,
                                       const char *role_id,
                                       const char *permission,
                                       const char *resource,
                                       UmiAuthorisationDecision *out_decision)
{
    UmiStudioOperations *operations;
    UmiSecurityContext *security;
    UmiSecurityEvent security_event = {0};
    UmiAuditRecord audit = {0};
    UmiStatus status;
    uint64_t now;

    if (services == NULL || principal == NULL || permission == NULL ||
        resource == NULL || out_decision == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    operations = studio_operations(services);
    security = umi_studio_operations_security(operations);
    if (security == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }
    now = umi_studio_operations_now(operations);
    status = umi_authorisation_check(
        umi_security_context_authorisation(security),
        principal,
        role_id,
        permission,
        resource,
        out_decision);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    security_event.timestamp_ns = now;
    (void)snprintf(security_event.principal,
                   sizeof(security_event.principal),
                   "%s",
                   principal);
    (void)snprintf(security_event.action,
                   sizeof(security_event.action),
                   "%s",
                   permission);
    (void)snprintf(security_event.resource,
                   sizeof(security_event.resource),
                   "%s",
                   resource);
    security_event.allowed = out_decision->allowed;
    status = umi_security_event_log_append(
        umi_security_context_events(security),
        &security_event);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    audit.timestamp_ns = now;
    (void)snprintf(audit.principal, sizeof(audit.principal), "%s", principal);
    (void)snprintf(audit.action, sizeof(audit.action), "%s", permission);
    (void)snprintf(audit.resource, sizeof(audit.resource), "%s", resource);
    audit.outcome = out_decision->allowed
        ? UMI_AUDIT_SUCCEEDED : UMI_AUDIT_DENIED;
    status = umi_audit_log_append(umi_studio_operations_audit(operations),
                                  &audit);
    if (status == UMI_STATUS_OK) {
        status = umi_metrics_counter_add(
            umi_studio_operations_metrics(operations),
            "studio.security.authorisation-checks",
            1.0,
            now);
    }
    return status;
}
