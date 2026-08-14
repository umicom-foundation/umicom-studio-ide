/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/operations.c
 *
 * PURPOSE:
 *   Compose Framework security, plug-in, observability and resilience services
 *   into one product-owned lifetime boundary used by Studio tools and frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/operations.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiStudioOperations {
    UmiClock *clock;
    UmiSecurityContext *security;
    UmiPluginHost *plugins;
    UmiMetricsRegistry *metrics;
    UmiTraceStore *traces;
    UmiAuditLog *audit;
    UmiProfiler *profiler;
    UmiReadinessRegistry *readiness;
    UmiOperationalEventLog *events;
    UmiResilienceSupervisor *resilience;
    UmiHealthGate *health_gate;
    UmiCircuitBreaker *plugin_circuit;
    UmiRateLimiter *tool_rate_limiter;
};

static UmiStatus register_studio_identity(UmiStudioOperations *operations)
{
    static const char *permissions[] = {
        "studio.session.write",
        "studio.documents.write",
        "studio.tasks.read",
        "studio.recovery.delete",
        "studio.workspace.read",
        "studio.workspace.write",
        "data.read",
        "messaging.publish",
        "messaging.replay",
        "studio.security.read",
        "studio.plugins.read",
        "studio.products.read",
        "studio.products.manage",
        "studio.ai.read",
        "studio.ai.manage",
        "studio.observability.read",
        "studio.resilience.read"
    };
    UmiIdentityRecord identity = {0};
    UmiRoleRegistry *roles;
    UmiIdentityRegistry *identities;
    UmiPolicyEngine *policy;
    UmiPolicyRule rule;
    UmiStatus status;
    size_t index;

    identities = umi_security_context_identities(operations->security);
    roles = umi_security_context_roles(operations->security);
    policy = umi_security_context_policy(operations->security);

    (void)snprintf(identity.identity_id,
                   sizeof(identity.identity_id),
                   "%s",
                   "studio.local-user");
    (void)snprintf(identity.display_name,
                   sizeof(identity.display_name),
                   "%s",
                   "Local Studio User");
    identity.kind = UMI_IDENTITY_HUMAN;
    identity.enabled = 1;
    identity.revision = 1U;

    status = umi_identity_registry_register(identities, &identity);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_role_registry_define(roles,
                                      "studio.developer",
                                      "Local developer using Umicom Studio IDE");
    if (status != UMI_STATUS_OK) {
        return status;
    }
    for (index = 0U; index < sizeof(permissions) / sizeof(permissions[0]); ++index) {
        status = umi_role_registry_grant(roles,
                                         "studio.developer",
                                         permissions[index]);
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    rule.principal = "studio.local-user";
    rule.capability = "studio.*";
    rule.resource = "*";
    rule.effect = UMI_POLICY_ALLOW;
    return umi_policy_engine_add(policy, &rule);
}

static UmiStatus configure_plugin_grants(UmiStudioOperations *operations)
{
    static const char *permissions[] = {
        "studio.commands",
        "studio.menus",
        "studio.panes",
        "studio.tools",
        "studio.providers",
        "studio.extensions.manage",
        "studio.extensions.install",
        "workspace.read"
    };
    UmiPermissionSet *grants = umi_plugin_host_grants(operations->plugins);
    UmiStatus status;
    size_t index;

    for (index = 0U; index < sizeof(permissions) / sizeof(permissions[0]); ++index) {
        status = umi_permission_set_add(grants, permissions[index]);
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    return UMI_STATUS_OK;
}

static UmiStatus configure_observability(UmiStudioOperations *operations)
{
    UmiOperationalEvent event = {0};
    UmiStatus status;
    uint64_t now = umi_studio_operations_now(operations);

    status = umi_metrics_registry_define(
        operations->metrics,
        "studio.security.authorisation-checks",
        "Number of Studio authorisation decisions",
        UMI_METRIC_COUNTER);
    if (status == UMI_STATUS_OK) {
        status = umi_metrics_registry_define(
            operations->metrics,
            "studio.plugins.discovered",
            "Number of discovered Studio plug-in manifests",
            UMI_METRIC_GAUGE);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_metrics_registry_define(
            operations->metrics,
            "studio.operations.events",
            "Number of Studio operational events",
            UMI_METRIC_COUNTER);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_readiness_registry_update(operations->readiness,
                                           "studio.security",
                                           1,
                                           "Security context is ready",
                                           now);
    if (status == UMI_STATUS_OK) {
        status = umi_readiness_registry_update(operations->readiness,
                                               "studio.plugins",
                                               1,
                                               "Plug-in host is ready",
                                               now);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_readiness_registry_update(operations->readiness,
                                               "studio.observability",
                                               1,
                                               "Observability registries are ready",
                                               now);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_readiness_registry_update(operations->readiness,
                                               "studio.resilience",
                                               1,
                                               "Resilience supervisor is ready",
                                               now);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }

    event.timestamp_ns = now;
    event.severity = UMI_DIAGNOSTIC_INFO;
    (void)snprintf(event.category, sizeof(event.category), "%s", "studio.lifecycle");
    (void)snprintf(event.message,
                   sizeof(event.message),
                   "%s",
                   "Studio operational services created");
    status = umi_operational_event_log_append(operations->events, &event);
    if (status == UMI_STATUS_OK) {
        status = umi_metrics_counter_add(operations->metrics,
                                         "studio.operations.events",
                                         1.0,
                                         now);
    }
    return status;
}

static UmiStatus configure_resilience(UmiStudioOperations *operations)
{
    static const char *components[] = {
        "studio.framework",
        "studio.data-server",
        "studio.integration-fabric",
        "studio.plugin-host"
    };
    UmiRestartPolicy policy = umi_restart_policy_default();
    UmiStatus status;
    size_t index;
    uint64_t now = umi_studio_operations_now(operations);

    for (index = 0U; index < sizeof(components) / sizeof(components[0]); ++index) {
        status = umi_resilience_supervisor_register(operations->resilience,
                                                    components[index],
                                                    &policy,
                                                    now);
        if (status != UMI_STATUS_OK) {
            return status;
        }
        status = umi_resilience_supervisor_success(operations->resilience,
                                                   components[index],
                                                   now);
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    return UMI_STATUS_OK;
}

static void destroy_partial(UmiStudioOperations *operations)
{
    if (operations == NULL) {
        return;
    }
    umi_rate_limiter_destroy(operations->tool_rate_limiter);
    umi_circuit_breaker_destroy(operations->plugin_circuit);
    umi_health_gate_destroy(operations->health_gate);
    umi_resilience_supervisor_destroy(operations->resilience);
    umi_operational_event_log_destroy(operations->events);
    umi_readiness_registry_destroy(operations->readiness);
    umi_profiler_destroy(operations->profiler);
    umi_audit_log_destroy(operations->audit);
    umi_trace_store_destroy(operations->traces);
    umi_metrics_registry_destroy(operations->metrics);
    umi_plugin_host_destroy(operations->plugins);
    umi_security_context_destroy(operations->security);
    free(operations);
}

UmiStatus umi_studio_operations_create(UmiClock *clock,
                                       UmiStudioOperations **out_operations)
{
    UmiCircuitBreakerConfig circuit_config;
    UmiRateLimiterConfig rate_config;
    UmiStudioOperations *operations;
    UmiStatus status;
    uint64_t now;

    if (clock == NULL || out_operations == NULL ||
        clock->wall_nanoseconds == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_operations = NULL;

    operations = (UmiStudioOperations *)calloc(1U, sizeof(*operations));
    if (operations == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    operations->clock = clock;
    now = umi_studio_operations_now(operations);

#define CREATE(call)                                                        \
    do {                                                                    \
        status = (call);                                                    \
        if (status != UMI_STATUS_OK) {                                      \
            destroy_partial(operations);                                    \
            return status;                                                  \
        }                                                                   \
    } while (0)

    CREATE(umi_security_context_create(&operations->security));
    CREATE(umi_plugin_host_create(&operations->plugins));
    CREATE(umi_metrics_registry_create(&operations->metrics));
    CREATE(umi_trace_store_create(&operations->traces));
    CREATE(umi_audit_log_create(&operations->audit));
    CREATE(umi_profiler_create(&operations->profiler));
    CREATE(umi_readiness_registry_create(&operations->readiness));
    CREATE(umi_operational_event_log_create(&operations->events));
    CREATE(umi_resilience_supervisor_create(&operations->resilience));
    CREATE(umi_health_gate_create(&operations->health_gate));

    circuit_config = umi_circuit_breaker_config_default();
    CREATE(umi_circuit_breaker_create(&circuit_config,
                                      &operations->plugin_circuit));

    rate_config.capacity = 50.0;
    rate_config.refill_per_second = 10.0;
    CREATE(umi_rate_limiter_create(&rate_config,
                                   now,
                                   &operations->tool_rate_limiter));
    CREATE(register_studio_identity(operations));
    CREATE(configure_plugin_grants(operations));
    CREATE(configure_observability(operations));
    CREATE(configure_resilience(operations));
#undef CREATE

    *out_operations = operations;
    return UMI_STATUS_OK;
}

void umi_studio_operations_destroy(UmiStudioOperations *operations)
{
    destroy_partial(operations);
}

#define GETTER(function_name, return_type, field_name)                      \
    return_type *function_name(UmiStudioOperations *operations)             \
    {                                                                       \
        return operations != NULL ? operations->field_name : NULL;          \
    }

GETTER(umi_studio_operations_security, UmiSecurityContext, security)
GETTER(umi_studio_operations_plugins, UmiPluginHost, plugins)
GETTER(umi_studio_operations_metrics, UmiMetricsRegistry, metrics)
GETTER(umi_studio_operations_traces, UmiTraceStore, traces)
GETTER(umi_studio_operations_audit, UmiAuditLog, audit)
GETTER(umi_studio_operations_profiler, UmiProfiler, profiler)
GETTER(umi_studio_operations_readiness, UmiReadinessRegistry, readiness)
GETTER(umi_studio_operations_events, UmiOperationalEventLog, events)
GETTER(umi_studio_operations_resilience, UmiResilienceSupervisor, resilience)
GETTER(umi_studio_operations_health_gate, UmiHealthGate, health_gate)
GETTER(umi_studio_operations_plugin_circuit, UmiCircuitBreaker, plugin_circuit)
GETTER(umi_studio_operations_tool_rate_limiter, UmiRateLimiter, tool_rate_limiter)
#undef GETTER

uint64_t umi_studio_operations_now(UmiStudioOperations *operations)
{
    return operations != NULL && operations->clock != NULL &&
           operations->clock->wall_nanoseconds != NULL
        ? operations->clock->wall_nanoseconds(operations->clock)
        : 0U;
}

UmiStatus umi_studio_operations_report(const UmiStudioOperations *operations,
                                       UmiStudioOperationsReport *out_report)
{
    UmiObservabilitySnapshot snapshot;
    UmiSecurityContext *security;
    UmiPluginHost *plugins;
    UmiStatus status;

    if (operations == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    security = operations->security;
    plugins = operations->plugins;
    (void)memset(out_report, 0, sizeof(*out_report));

    status = umi_observability_snapshot_capture(operations->metrics,
                                                operations->traces,
                                                operations->audit,
                                                operations->readiness,
                                                operations->events,
                                                &snapshot);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    out_report->identities = umi_identity_registry_count(
        umi_security_context_identities(security));
    out_report->roles = umi_role_registry_count(
        umi_security_context_roles(security));
    out_report->sessions = umi_session_registry_count(
        umi_security_context_sessions(security));
    out_report->trusted_workspaces = umi_workspace_trust_store_count(
        umi_security_context_workspace_trust(security));
    out_report->security_events = umi_security_event_log_count(
        umi_security_context_events(security));
    out_report->denied_security_events = umi_security_event_log_denied_count(
        umi_security_context_events(security));
    out_report->plugins = umi_plugin_registry_count(
        umi_plugin_host_registry(plugins));
    out_report->plugin_contributions = umi_plugin_contribution_registry_count(
        umi_plugin_host_contributions(plugins));
    out_report->metrics = snapshot.metrics;
    out_report->trace_spans = snapshot.spans;
    out_report->audit_records = snapshot.audit_records;
    out_report->readiness_checks = snapshot.readiness_checks;
    out_report->operational_events = snapshot.operational_events;
    out_report->supervised_components = umi_resilience_supervisor_count(
        operations->resilience);
    out_report->ready = snapshot.ready;
    return UMI_STATUS_OK;
}
