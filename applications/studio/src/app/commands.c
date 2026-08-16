/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/commands.c
 *
 * PURPOSE:
 *   Register Studio session, document, task and recovery use cases once so
 *   menus, shortcuts, command palettes, consoles and automation use the same
 *   typed command identifiers and handlers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/commands.h"
#include "umicom/studio/knowledge_commands.h"

#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/studio/build.h"
#include "umicom/studio/coding_assistant.h"
#include "umicom/studio/data.h"
#include "umicom/studio/debugger.h"
#include "umicom/studio/developer_platform.h"
#include "umicom/studio/diagnostics.h"
#include "umicom/studio/documents.h"
#include "umicom/studio/language.h"
#include "umicom/studio/messages.h"
#include "umicom/studio/observability.h"
#include "umicom/studio/plugins.h"
#include "umicom/studio/product_centre.h"
#include "umicom/studio/resilience.h"
#include "umicom/studio/security.h"
#include "umicom/studio/source_control.h"
#include "umicom/studio/terminal.h"
#include "umicom/studio/tests.h"
#include "umicom/studio/trading.h"
#include "umicom/studio/replay.h"
#include "umicom/studio/session.h"
#include "umicom/studio/watcher.h"
#include "umicom/studio/workspace.h"

static UmiStatus session_save_handler(void *user_data,
                                      const char *argument,
                                      char *out_message,
                                      size_t message_capacity)
{
    UmiStatus status;
    (void)argument;
    status = umi_studio_session_save((UmiStudioServices *)user_data);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "%s",
                       status == UMI_STATUS_OK
                           ? "Studio session saved"
                           : umi_status_text(status));
    }
    return status;
}

static UmiStatus documents_save_all_handler(void *user_data,
                                            const char *argument,
                                            char *out_message,
                                            size_t message_capacity)
{
    size_t saved = 0U;
    size_t failed = 0U;
    UmiStatus status;
    (void)argument;

    status = umi_studio_documents_save_all((UmiStudioServices *)user_data,
                                           &saved,
                                           &failed);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "Saved %zu document(s); %zu failed",
                       saved,
                       failed);
    }
    return status;
}

static UmiStatus tasks_wait_idle_handler(void *user_data,
                                         const char *argument,
                                         char *out_message,
                                         size_t message_capacity)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiTaskQueue *queue = umi_studio_services_task_queue(services);
    unsigned long timeout = 0UL;
    char *end = NULL;
    UmiStatus status;

    if (argument != NULL && argument[0] != '\0') {
        errno = 0;
        timeout = strtoul(argument, &end, 10);
        if (errno != 0 || end == argument || *end != '\0' ||
            timeout > UINT32_MAX) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    status = umi_task_queue_wait_idle(queue, (uint32_t)timeout);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "%s",
                       status == UMI_STATUS_OK
                           ? "Studio task queue is idle"
                           : umi_status_text(status));
    }
    return status;
}

static UmiStatus recovery_purge_handler(void *user_data,
                                        const char *argument,
                                        char *out_message,
                                        size_t message_capacity)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiStatus status;
    (void)argument;

    status = umi_recovery_manager_purge(
        umi_studio_services_recovery(services)
    );
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "%s",
                       status == UMI_STATUS_OK
                           ? "Studio recovery data purged"
                           : umi_status_text(status));
    }
    return status;
}

static UmiStatus workspace_refresh_handler(void *user_data,
                                           const char *argument,
                                           char *out_message,
                                           size_t message_capacity)
{
    UmiStatus status;
    (void)argument;
    status = umi_studio_workspace_refresh((UmiStudioServices *)user_data);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "%s",
                       status == UMI_STATUS_OK
                           ? "Studio workspace refreshed"
                           : umi_status_text(status));
    }
    return status;
}

static UmiStatus workspace_close_handler(void *user_data,
                                         const char *argument,
                                         char *out_message,
                                         size_t message_capacity)
{
    UmiStatus status;
    (void)argument;
    status = umi_studio_workspace_close((UmiStudioServices *)user_data);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "%s",
                       status == UMI_STATUS_OK
                           ? "Studio workspace closed"
                           : umi_status_text(status));
    }
    return status;
}

static UmiStatus watcher_scan_handler(void *user_data,
                                      const char *argument,
                                      char *out_message,
                                      size_t message_capacity)
{
    UmiStatus status;
    (void)argument;
    status = umi_studio_watcher_scan_once((UmiStudioServices *)user_data);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "%s",
                       status == UMI_STATUS_OK
                           ? "Studio workspace watcher scanned"
                           : umi_status_text(status));
    }
    return status;
}


static UmiStatus data_integrity_handler(void *user_data,
                                        const char *argument,
                                        char *out_message,
                                        size_t message_capacity)
{
    UmiStudioDataReport report;
    UmiStatus status;
    (void)argument;
    status = umi_studio_data_report((UmiStudioServices *)user_data, &report);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
                       "Data Server %s: %zu record(s), integrity %s",
                       report.backend_name,
                       report.records,
                       umi_status_text(report.integrity_status));
    }
    return status;
}

static UmiStatus messages_flush_handler(void *user_data,
                                         const char *argument,
                                         char *out_message,
                                         size_t message_capacity)
{
    size_t delivered = 0U;
    size_t failed = 0U;
    UmiStatus status;
    (void)argument;
    status = umi_studio_messages_flush_outbox((UmiStudioServices *)user_data,
                                              0U,
                                              &delivered,
                                              &failed);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
                       "Outbox delivered %zu; failed %zu",
                       delivered, failed);
    }
    return status;
}

static UmiStatus messages_replay_handler(void *user_data,
                                          const char *argument,
                                          char *out_message,
                                          size_t message_capacity)
{
    UmiStudioReplayRequest request = umi_studio_replay_request_default();
    size_t replayed = 0U;
    UmiStatus status;
    (void)argument;
    status = umi_studio_replay((UmiStudioServices *)user_data,
                               &request,
                               &replayed);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
                       "Replayed %zu durable message(s)", replayed);
    }
    return status;
}

static UmiStatus security_report_handler(void *user_data,
                                         const char *argument,
                                         char *out_message,
                                         size_t message_capacity)
{
    UmiStudioSecurityReport report;
    UmiStatus status;
    (void)argument;
    status = umi_studio_security_report((UmiStudioServices *)user_data,
                                        &report);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "Security: %zu identities, %zu roles, %zu events",
                       report.identities,
                       report.roles,
                       report.events);
    }
    return status;
}

static UmiStatus plugins_report_handler(void *user_data,
                                        const char *argument,
                                        char *out_message,
                                        size_t message_capacity)
{
    UmiStudioPluginReport report;
    UmiStatus status;
    (void)argument;
    status = umi_studio_plugins_report((UmiStudioServices *)user_data,
                                       &report);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "Plug-ins: %zu registered, %zu contributions",
                       report.registered,
                       report.contributions);
    }
    return status;
}

static UmiStatus marketplace_check_updates_handler(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiStudioProductCentre *centre = umi_studio_services_product_centre(services);
    UmiClock *clock = umi_studio_services_clock(services);
    size_t available = 0U;
    UmiStatus status;
    uint64_t timestamp_ms = 0U;
    (void)argument;

    if (clock != NULL && clock->wall_nanoseconds != NULL) {
        timestamp_ms = clock->wall_nanoseconds(clock) / UINT64_C(1000000);
    }
    status = centre != NULL
        ? umi_studio_product_centre_check_updates(
              centre, timestamp_ms, &available)
        : UMI_STATUS_UNAVAILABLE;
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
                       "Product updates: %zu verified release(s) available",
                       available);
    }
    return status;
}

static UmiStatus marketplace_plan_update_handler(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiStudioProductCentre *centre = umi_studio_services_product_centre(services);
    UmiClock *clock = umi_studio_services_clock(services);
    UmiDistributionDecision decision;
    UmiDistributionTransaction transaction;
    UmiStatus status;
    uint64_t timestamp_ms = 0U;
    if (argument == NULL || argument[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(&decision, 0, sizeof(decision));
    (void)memset(&transaction, 0, sizeof(transaction));
    if (clock != NULL && clock->wall_nanoseconds != NULL) {
        timestamp_ms = clock->wall_nanoseconds(clock) / UINT64_C(1000000);
    }
    status = centre != NULL
        ? umi_studio_product_centre_plan_update(
              centre, argument, timestamp_ms, &decision, &transaction)
        : UMI_STATUS_UNAVAILABLE;
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s",
                       status == UMI_STATUS_OK
                           ? transaction.message
                           : (decision.reason[0] != '\0'
                                  ? decision.reason : umi_status_text(status)));
    }
    return status;
}

static UmiStatus observability_report_handler(void *user_data,
                                              const char *argument,
                                              char *out_message,
                                              size_t message_capacity)
{
    UmiStudioObservabilityReport report;
    UmiStatus status;
    (void)argument;
    status = umi_studio_observability_report((UmiStudioServices *)user_data,
                                             &report);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "Observability: %zu metrics, %zu events, ready %s",
                       report.snapshot.metrics,
                       report.snapshot.operational_events,
                       report.snapshot.ready ? "yes" : "no");
    }
    return status;
}

static UmiStatus resilience_report_handler(void *user_data,
                                           const char *argument,
                                           char *out_message,
                                           size_t message_capacity)
{
    UmiStudioResilienceReport report;
    UmiStatus status;
    (void)argument;
    status = umi_studio_resilience_report((UmiStudioServices *)user_data,
                                          &report);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "Resilience: %zu supervised, %zu running, %zu failed",
                       report.supervised_components,
                       report.running_components,
                       report.failed_components);
    }
    return status;
}

static UmiStatus build_phase_handler(UmiStudioServices *services,
                                     UmiBuildPhase phase,
                                     char *out_message,
                                     size_t message_capacity)
{
    UmiBuildResult *result = NULL;
    UmiStatus status = umi_build_result_create(&result);
    UmiStatus diagnostic_status;
    if (status != UMI_STATUS_OK) return status;
    status = umi_studio_build_service_run(
        umi_studio_services_build(services), phase, result);
    diagnostic_status =
        umi_studio_diagnostics_ingest_build_result(services, result);
    if (status == UMI_STATUS_OK && diagnostic_status != UMI_STATUS_OK) {
        status = diagnostic_status;
    }
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "%s: %s (exit %d, %zu diagnostic(s))",
                       umi_build_phase_text(phase),
                       umi_status_text(status),
                       result->exit_code,
                       result->diagnostics.count);
    }
    umi_build_result_destroy(result);
    return status;
}

static UmiStatus build_configure_handler(void *user_data,
                                         const char *argument,
                                         char *out_message,
                                         size_t message_capacity)
{
    (void)argument;
    return build_phase_handler((UmiStudioServices *)user_data,
                               UMI_BUILD_PHASE_CONFIGURE,
                               out_message,
                               message_capacity);
}

static UmiStatus build_compile_handler(void *user_data,
                                       const char *argument,
                                       char *out_message,
                                       size_t message_capacity)
{
    (void)argument;
    return build_phase_handler((UmiStudioServices *)user_data,
                               UMI_BUILD_PHASE_BUILD,
                               out_message,
                               message_capacity);
}

static UmiStatus build_test_handler(void *user_data,
                                    const char *argument,
                                    char *out_message,
                                    size_t message_capacity)
{
    (void)argument;
    return build_phase_handler((UmiStudioServices *)user_data,
                               UMI_BUILD_PHASE_TEST,
                               out_message,
                               message_capacity);
}

static UmiStatus build_clean_handler(void *user_data,
                                     const char *argument,
                                     char *out_message,
                                     size_t message_capacity)
{
    (void)argument;
    return build_phase_handler((UmiStudioServices *)user_data,
                               UMI_BUILD_PHASE_CLEAN,
                               out_message,
                               message_capacity);
}

static UmiStatus build_run_handler(void *user_data,
                                   const char *argument,
                                   char *out_message,
                                   size_t message_capacity)
{
    (void)argument;
    return build_phase_handler((UmiStudioServices *)user_data,
                               UMI_BUILD_PHASE_RUN,
                               out_message,
                               message_capacity);
}

static UmiStatus build_install_handler(void *user_data,
                                       const char *argument,
                                       char *out_message,
                                       size_t message_capacity)
{
    (void)argument;
    return build_phase_handler((UmiStudioServices *)user_data,
                               UMI_BUILD_PHASE_INSTALL,
                               out_message,
                               message_capacity);
}

static UmiStatus build_retry_handler(void *user_data,
                                     const char *argument,
                                     char *out_message,
                                     size_t message_capacity)
{
    UmiStudioBuildService *service = umi_studio_services_build(
        (UmiStudioServices *)user_data);
    UmiBuildWorkspaceSnapshot snapshot;
    const char *node_id = argument;
    UmiStatus status;
    if (node_id == NULL || node_id[0] == '\0') {
        status = umi_build_workspace_snapshot(
            umi_studio_build_service_workspace(service), &snapshot);
        if (status != UMI_STATUS_OK || !snapshot.has_selected_node)
            return status != UMI_STATUS_OK
                ? status : UMI_STATUS_INVALID_ARGUMENT;
        node_id = snapshot.selected_node_id;
    }
    status = umi_studio_build_service_retry(service, node_id);
    if (out_message != NULL && message_capacity > 0U) {
        if (status == UMI_STATUS_OK) {
            (void)snprintf(out_message, message_capacity,
                           "Operation '%s' scheduled for retry", node_id);
        } else {
            (void)snprintf(out_message, message_capacity,
                           "Retry '%s': %s", node_id,
                           umi_status_text(status));
        }
    }
    return status;
}

static UmiStatus build_cancel_handler(void *user_data,
                                      const char *argument,
                                      char *out_message,
                                      size_t message_capacity)
{
    (void)argument;
    umi_studio_build_service_cancel(
        umi_studio_services_build((UmiStudioServices *)user_data));
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
                       "Build cancellation requested");
    }
    return UMI_STATUS_OK;
}

static UmiBuildWorkspace *build_workspace(void *user_data)
{
    UmiStudioBuildService *service = umi_studio_services_build(
        (UmiStudioServices *)user_data);
    return service != NULL
        ? umi_studio_build_service_workspace(service) : NULL;
}

static UmiStatus build_filter_handler(void *user_data,
                                      const char *argument,
                                      char *out_message,
                                      size_t message_capacity)
{
    UmiBuildWorkspace *workspace = build_workspace(user_data);
    UmiBuildWorkspaceNodeFilter node_filter = UMI_BUILD_WORKSPACE_NODES_ALL;
    const char *text = argument != NULL ? argument : "";
    UmiBuildWorkspaceSnapshot snapshot;
    UmiStatus status;

    if (workspace == NULL) return UMI_STATUS_UNAVAILABLE;
    if (strcmp(text, "all") == 0) text = "";
    else if (strcmp(text, "pending") == 0) {
        text = "";
        node_filter = UMI_BUILD_WORKSPACE_NODES_PENDING;
    } else if (strcmp(text, "ready") == 0) {
        text = "";
        node_filter = UMI_BUILD_WORKSPACE_NODES_READY;
    } else if (strcmp(text, "running") == 0) {
        text = "";
        node_filter = UMI_BUILD_WORKSPACE_NODES_RUNNING;
    } else if (strcmp(text, "succeeded") == 0) {
        text = "";
        node_filter = UMI_BUILD_WORKSPACE_NODES_SUCCEEDED;
    } else if (strcmp(text, "failed") == 0) {
        text = "";
        node_filter = UMI_BUILD_WORKSPACE_NODES_FAILED;
    } else if (strcmp(text, "blocked") == 0) {
        text = "";
        node_filter = UMI_BUILD_WORKSPACE_NODES_BLOCKED;
    }
    status = umi_build_workspace_set_filter(workspace, text, node_filter);
    if (status == UMI_STATUS_OK)
        status = umi_build_workspace_snapshot(workspace, &snapshot);
    if (out_message != NULL && message_capacity > 0U) {
        if (status == UMI_STATUS_OK) {
            (void)snprintf(out_message, message_capacity,
                           "Build filter selected %zu build node(s)",
                           snapshot.visible_node_count);
        } else {
            (void)snprintf(out_message, message_capacity, "Build filter: %s",
                           umi_status_text(status));
        }
    }
    return status;
}

static UmiStatus build_select_node_handler(void *user_data,
                                           const char *argument,
                                           char *out_message,
                                           size_t message_capacity)
{
    UmiBuildWorkspace *workspace = build_workspace(user_data);
    UmiStatus status;

    if (workspace == NULL) return UMI_STATUS_UNAVAILABLE;
    if (argument == NULL || argument[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_workspace_select_node(workspace, argument);
    if (out_message != NULL && message_capacity > 0U)
        (void)snprintf(out_message, message_capacity,
                       "Selected build node %s: %s", argument,
                       umi_status_text(status));
    return status;
}

static UmiStatus build_select_operation_handler(void *user_data,
                                                const char *argument,
                                                char *out_message,
                                                size_t message_capacity)
{
    UmiBuildWorkspace *workspace = build_workspace(user_data);
    unsigned long long operation_id;
    char *end = NULL;
    UmiStatus status;

    if (workspace == NULL) return UMI_STATUS_UNAVAILABLE;
    if (argument == NULL || argument[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    errno = 0;
    operation_id = strtoull(argument, &end, 10);
    if (errno != 0 || end == argument || *end != '\0' || operation_id == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_workspace_select_operation(
        workspace, (uint64_t)operation_id);
    if (out_message != NULL && message_capacity > 0U)
        (void)snprintf(out_message, message_capacity,
                       "Selected build operation %llu: %s", operation_id,
                       umi_status_text(status));
    return status;
}

static UmiStatus build_select_artifact_handler(void *user_data,
                                               const char *argument,
                                               char *out_message,
                                               size_t message_capacity)
{
    UmiBuildWorkspace *workspace = build_workspace(user_data);
    UmiStatus status;

    if (workspace == NULL) return UMI_STATUS_UNAVAILABLE;
    if (argument == NULL || argument[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_workspace_select_artifact(workspace, argument);
    if (out_message != NULL && message_capacity > 0U)
        (void)snprintf(out_message, message_capacity,
                       "Selected build artifact %s: %s", argument,
                       umi_status_text(status));
    return status;
}

static UmiStatus build_run_next_handler(void *user_data,
                                        const char *argument,
                                        char *out_message,
                                        size_t message_capacity)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiBuildResult *result = NULL;
    UmiStatus status;
    (void)argument;

    status = umi_build_result_create(&result);
    if (status != UMI_STATUS_OK) return status;
    status = umi_studio_build_service_execute_next(
        umi_studio_services_build(services), result);
    if (status != UMI_STATUS_NOT_FOUND) {
        UmiStatus diagnostic_status =
            umi_studio_diagnostics_ingest_build_result(services, result);
        if (status == UMI_STATUS_OK && diagnostic_status != UMI_STATUS_OK)
            status = diagnostic_status;
    }
    if (out_message != NULL && message_capacity > 0U) {
        if (status == UMI_STATUS_NOT_FOUND) {
            (void)snprintf(out_message, message_capacity,
                           "No build graph node is ready");
        } else {
            (void)snprintf(out_message, message_capacity,
                           "Build operation #%" PRIu64 ": %s",
                           result->operation_id, umi_status_text(status));
        }
    }
    umi_build_result_destroy(result);
    return status;
}

static UmiStatus build_run_all_handler(void *user_data,
                                       const char *argument,
                                       char *out_message,
                                       size_t message_capacity)
{
    UmiStudioBuildService *service = umi_studio_services_build(
        (UmiStudioServices *)user_data);
    unsigned long maximum_nodes = UMI_BUILD_GRAPH_MAX_NODES;
    char *end = NULL;
    size_t executed = 0U;
    UmiStatus status;

    if (argument != NULL && argument[0] != '\0') {
        errno = 0;
        maximum_nodes = strtoul(argument, &end, 10);
        if (errno != 0 || end == argument || *end != '\0' ||
            maximum_nodes == 0UL || maximum_nodes > UMI_BUILD_GRAPH_MAX_NODES)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_studio_build_service_execute_all(
        service, (size_t)maximum_nodes, &executed);
    if (executed > 0U) {
        UmiBuildResult *latest = NULL;
        if (umi_build_result_create(&latest) == UMI_STATUS_OK &&
            umi_build_history_latest(
                umi_studio_build_service_history(service), latest) ==
                UMI_STATUS_OK) {
            UmiStatus diagnostic_status =
                umi_studio_diagnostics_ingest_build_result(
                    (UmiStudioServices *)user_data, latest);
            if (status == UMI_STATUS_OK && diagnostic_status != UMI_STATUS_OK)
                status = diagnostic_status;
        }
        umi_build_result_destroy(latest);
    }
    if (out_message != NULL && message_capacity > 0U)
        (void)snprintf(out_message, message_capacity,
                       "Build plan executed %zu node(s): %s", executed,
                       umi_status_text(status));
    return status;
}

static UmiStatus build_invalidate_handler(void *user_data,
                                          const char *argument,
                                          char *out_message,
                                          size_t message_capacity)
{
    UmiStudioBuildService *service = umi_studio_services_build(
        (UmiStudioServices *)user_data);
    UmiBuildWorkspace *workspace = service != NULL
        ? umi_studio_build_service_workspace(service) : NULL;
    UmiBuildWorkspaceSnapshot snapshot;
    UmiBuildGraphNodeSnapshot node;
    const char *node_id = argument;
    UmiStatus status;

    if (workspace == NULL) return UMI_STATUS_UNAVAILABLE;
    status = umi_build_workspace_snapshot(workspace, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (node_id == NULL || node_id[0] == '\0')
        node_id = snapshot.selected_node_id;
    if (node_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_graph_find(umi_studio_build_service_graph(service),
                                  node_id, &node);
    if (status == UMI_STATUS_OK)
        status = umi_studio_build_service_invalidate(
            service, node_id, node.input_revision + 1U);
    if (out_message != NULL && message_capacity > 0U)
        (void)snprintf(out_message, message_capacity,
                       "Invalidate build node %s: %s", node_id,
                       umi_status_text(status));
    return status;
}

static UmiStatus build_refresh_handler(void *user_data,
                                       const char *argument,
                                       char *out_message,
                                       size_t message_capacity)
{
    UmiBuildWorkspace *workspace = build_workspace(user_data);
    UmiBuildWorkspaceSnapshot snapshot;
    UmiStatus status;
    (void)argument;

    if (workspace == NULL) return UMI_STATUS_UNAVAILABLE;
    status = umi_build_workspace_refresh(workspace);
    if (status == UMI_STATUS_OK)
        status = umi_build_workspace_snapshot(workspace, &snapshot);
    if (out_message != NULL && message_capacity > 0U) {
        if (status == UMI_STATUS_OK) {
            (void)snprintf(out_message, message_capacity,
                           "Build workspace refreshed: %zu ready, %zu running, %zu failed",
                           snapshot.graph.ready_count,
                           snapshot.graph.running_count,
                           snapshot.graph.failed_count);
        } else {
            (void)snprintf(out_message, message_capacity,
                           "Build refresh: %s", umi_status_text(status));
        }
    }
    return status;
}

static UmiStatus build_clear_history_handler(void *user_data,
                                             const char *argument,
                                             char *out_message,
                                             size_t message_capacity)
{
    UmiBuildWorkspace *workspace = build_workspace(user_data);
    (void)argument;

    if (workspace == NULL) return UMI_STATUS_UNAVAILABLE;
    umi_build_workspace_clear_history(workspace);
    if (out_message != NULL && message_capacity > 0U)
        (void)snprintf(out_message, message_capacity,
                       "Build history and retained output cleared");
    return UMI_STATUS_OK;
}

static UmiStatus tests_discover_handler(void *user_data,
                                        const char *argument,
                                        char *out_message,
                                        size_t message_capacity)
{
    UmiTestPlatformCtestImportSummary summary;
    const UmiBuildProfile *profile;
    UmiStatus status;
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    profile = umi_studio_build_service_profile(
        umi_studio_services_build(services));
    status = umi_studio_test_service_discover_metadata(
        umi_studio_services_tests(services),
        profile->source_directory, "studio",
        argument != NULL && argument[0] != '\0'
            ? argument : profile->build_directory,
        profile->configuration, &summary);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "Discovered %zu test(s): %s",
                       status == UMI_STATUS_OK ? summary.discovered_count : 0U,
                       umi_status_text(status));
    }
    return status;
}

static UmiTestWorkspace *tests_workspace(void *user_data)
{
    UmiStudioTestService *service = umi_studio_services_tests(
        (UmiStudioServices *)user_data);
    return service != NULL
        ? umi_studio_test_service_workspace(service) : NULL;
}

static UmiStatus tests_filter_handler(void *user_data,
                                      const char *argument,
                                      char *out_message,
                                      size_t message_capacity)
{
    UmiTestWorkspace *workspace = tests_workspace(user_data);
    const char *text = argument != NULL ? argument : "";
    int outcome = -1;
    int failed_only = 0;
    UmiStatus status;

    if (workspace == NULL) return UMI_STATUS_UNAVAILABLE;
    if (strcmp(text, "all") == 0) text = "";
    else if (strcmp(text, "failed") == 0) failed_only = 1;
    else if (strcmp(text, "passed") == 0) {
        text = "";
        outcome = UMI_TEST_PLATFORM_OUTCOME_PASSED;
    } else if (strcmp(text, "skipped") == 0) {
        text = "";
        outcome = UMI_TEST_PLATFORM_OUTCOME_SKIPPED;
    } else if (strcmp(text, "not-run") == 0) {
        text = "";
        outcome = UMI_TEST_PLATFORM_OUTCOME_NOT_RUN;
    }
    status = umi_test_workspace_set_filter(
        workspace, failed_only ? "" : text, "", "", outcome, 0,
        failed_only);
    if (out_message != NULL && message_capacity > 0U) {
        UmiTestWorkspaceSnapshot snapshot;
        if (status == UMI_STATUS_OK &&
            umi_test_workspace_snapshot(workspace, &snapshot) ==
                UMI_STATUS_OK) {
            (void)snprintf(out_message, message_capacity,
                           "Testing filter selected %zu test(s)",
                           snapshot.visible_item_count);
        } else {
            (void)snprintf(out_message, message_capacity, "Testing filter: %s",
                           umi_status_text(status));
        }
    }
    return status;
}

static UmiStatus tests_select_handler(void *user_data,
                                      const char *argument,
                                      char *out_message,
                                      size_t message_capacity)
{
    UmiTestWorkspace *workspace = tests_workspace(user_data);
    UmiStatus status;

    if (workspace == NULL) return UMI_STATUS_UNAVAILABLE;
    if (argument == NULL || argument[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_test_workspace_select_item(workspace, argument);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "Selected test %s: %s",
                       argument, umi_status_text(status));
    }
    return status;
}

typedef enum StudioTestWorkspaceOperation {
    STUDIO_TEST_RUN_ALL,
    STUDIO_TEST_RUN_SELECTED,
    STUDIO_TEST_DEBUG_SELECTED,
    STUDIO_TEST_RUN_COVERAGE,
    STUDIO_TEST_RERUN_FAILED,
    STUDIO_TEST_REPEAT_SELECTED
} StudioTestWorkspaceOperation;

static UmiStatus tests_execute_operation(
    void *user_data, const char *argument, char *out_message,
    size_t message_capacity, StudioTestWorkspaceOperation operation)
{
    UmiStudioTestService *service = umi_studio_services_tests(
        (UmiStudioServices *)user_data);
    UmiTestWorkspace *workspace = service != NULL
        ? umi_studio_test_service_workspace(service) : NULL;
    UmiTestPlatformOperationPlan plan;
    UmiTestPlatformExecutionSummary summary;
    UmiStatus status;

    if (workspace == NULL) return UMI_STATUS_UNAVAILABLE;
    switch (operation) {
        case STUDIO_TEST_RUN_ALL:
            status = umi_test_workspace_set_run_mode(
                workspace, UMI_TEST_WORKSPACE_RUN);
            if (status == UMI_STATUS_OK) {
                status = umi_test_workspace_plan_all(workspace, &plan);
            }
            break;
        case STUDIO_TEST_RUN_SELECTED:
            status = umi_test_workspace_set_run_mode(
                workspace, UMI_TEST_WORKSPACE_RUN);
            if (status == UMI_STATUS_OK) {
                status = umi_test_workspace_plan_selected(workspace, &plan);
            }
            break;
        case STUDIO_TEST_DEBUG_SELECTED:
            status = umi_test_workspace_set_run_mode(
                workspace, UMI_TEST_WORKSPACE_DEBUG);
            if (status == UMI_STATUS_OK) {
                status = umi_test_workspace_plan_selected(workspace, &plan);
            }
            break;
        case STUDIO_TEST_RUN_COVERAGE:
            status = umi_test_workspace_set_run_mode(
                workspace, UMI_TEST_WORKSPACE_COVERAGE);
            if (status == UMI_STATUS_OK) {
                status = umi_test_workspace_plan_all(workspace, &plan);
            }
            break;
        case STUDIO_TEST_RERUN_FAILED:
            status = umi_test_workspace_set_run_mode(
                workspace, UMI_TEST_WORKSPACE_RUN);
            if (status == UMI_STATUS_OK) {
                status = umi_test_workspace_plan_failed(workspace, &plan);
            }
            break;
        case STUDIO_TEST_REPEAT_SELECTED: {
            char *end = NULL;
            unsigned long repeat = argument != NULL && argument[0] != '\0'
                ? strtoul(argument, &end, 10) : 10UL;
            if (repeat == 0UL || repeat > UINT32_MAX ||
                (end != NULL && *end != '\0')) {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            status = umi_test_workspace_set_run_mode(
                workspace, UMI_TEST_WORKSPACE_RUN);
            if (status == UMI_STATUS_OK) {
                status = umi_test_workspace_plan_repeat_selected(
                    workspace, (uint32_t)repeat, 0, &plan);
            }
            break;
        }
        default:
            status = UMI_STATUS_INVALID_ARGUMENT;
            break;
    }
    (void)memset(&summary, 0, sizeof(summary));
    if (status == UMI_STATUS_OK) {
        status = umi_studio_test_service_execute(service, &plan, &summary);
    }
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
                       "Tests: %zu planned, %zu executed, %zu passed, "
                       "%zu failed (%s)",
                       summary.planned, summary.executed, summary.passed,
                       summary.failed, umi_status_text(status));
    }
    return status;
}

#define DEFINE_TEST_EXECUTION_HANDLER(name_, operation_)                    \
    static UmiStatus name_(void *user_data, const char *argument,           \
                           char *out_message, size_t capacity)              \
    {                                                                       \
        return tests_execute_operation(user_data, argument, out_message,   \
                                       capacity, operation_);              \
    }
DEFINE_TEST_EXECUTION_HANDLER(tests_run_all_handler, STUDIO_TEST_RUN_ALL)
DEFINE_TEST_EXECUTION_HANDLER(tests_run_selected_handler,
                              STUDIO_TEST_RUN_SELECTED)
DEFINE_TEST_EXECUTION_HANDLER(tests_debug_selected_handler,
                              STUDIO_TEST_DEBUG_SELECTED)
DEFINE_TEST_EXECUTION_HANDLER(tests_run_coverage_handler,
                              STUDIO_TEST_RUN_COVERAGE)
DEFINE_TEST_EXECUTION_HANDLER(tests_rerun_failed_handler,
                              STUDIO_TEST_RERUN_FAILED)
DEFINE_TEST_EXECUTION_HANDLER(tests_repeat_selected_handler,
                              STUDIO_TEST_REPEAT_SELECTED)
#undef DEFINE_TEST_EXECUTION_HANDLER

static UmiStatus tests_stop_handler(void *user_data, const char *argument,
                                    char *out_message, size_t capacity)
{
    UmiTestWorkspace *workspace = tests_workspace(user_data);
    UmiStatus status;
    (void)argument;

    status = workspace != NULL
        ? umi_test_workspace_request_stop(workspace) : UMI_STATUS_UNAVAILABLE;
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "Stop test run: %s",
                       umi_status_text(status));
    }
    return status;
}

typedef enum StudioTestClearOperation {
    STUDIO_TEST_CLEAR_RESULTS,
    STUDIO_TEST_CLEAR_OUTPUT,
    STUDIO_TEST_CLEAR_COVERAGE
} StudioTestClearOperation;

static UmiStatus tests_clear_operation(
    void *user_data, const char *argument, char *out_message, size_t capacity,
    StudioTestClearOperation operation)
{
    UmiTestWorkspace *workspace = tests_workspace(user_data);
    (void)argument;

    if (workspace == NULL) return UMI_STATUS_UNAVAILABLE;
    if (operation == STUDIO_TEST_CLEAR_RESULTS) {
        umi_test_workspace_clear_results(workspace);
    } else if (operation == STUDIO_TEST_CLEAR_OUTPUT) {
        umi_test_workspace_clear_output(workspace);
    } else {
        umi_test_workspace_clear_coverage(workspace);
    }
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "Testing evidence cleared");
    }
    return UMI_STATUS_OK;
}

#define DEFINE_TEST_CLEAR_HANDLER(name_, operation_)                        \
    static UmiStatus name_(void *user_data, const char *argument,           \
                           char *out_message, size_t capacity)              \
    {                                                                       \
        return tests_clear_operation(user_data, argument, out_message,     \
                                     capacity, operation_);                \
    }
DEFINE_TEST_CLEAR_HANDLER(tests_clear_results_handler,
                          STUDIO_TEST_CLEAR_RESULTS)
DEFINE_TEST_CLEAR_HANDLER(tests_clear_output_handler, STUDIO_TEST_CLEAR_OUTPUT)
DEFINE_TEST_CLEAR_HANDLER(tests_clear_coverage_handler,
                          STUDIO_TEST_CLEAR_COVERAGE)
#undef DEFINE_TEST_CLEAR_HANDLER

static UmiStatus terminal_execute_handler(void *user_data,
                                          const char *argument,
                                          char *out_message,
                                          size_t message_capacity)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    int exit_code = 0;
    UmiStatus status;
    if (argument == NULL || argument[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_terminal_controller_execute(
        umi_studio_services_terminal_controller(services),
        argument,
        30000U,
        NULL,
        &exit_code);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "Terminal command exited with %d: %s",
                       exit_code,
                       umi_status_text(status));
    }
    return status;
}

static UmiStatus diagnostics_clear_handler(void *user_data,
                                           const char *argument,
                                           char *out_message,
                                           size_t message_capacity)
{
    UmiStatus status;
    (void)argument;
    status = umi_studio_diagnostics_clear_problems((UmiStudioServices *)user_data);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s",
                       status == UMI_STATUS_OK ? "Problems cleared" : umi_status_text(status));
    }
    return status;
}

static UmiStatus output_clear_handler(void *user_data,
                                      const char *argument,
                                      char *out_message,
                                      size_t message_capacity)
{
    UmiStatus status;
    (void)argument;
    status = umi_studio_diagnostics_clear_output((UmiStudioServices *)user_data);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s",
                       status == UMI_STATUS_OK ? "Output cleared" : umi_status_text(status));
    }
    return status;
}

static UmiStatus terminal_clear_handler(void *user_data,
                                        const char *argument,
                                        char *out_message,
                                        size_t message_capacity)
{
    UmiTerminalController *controller = umi_studio_services_terminal_controller(
        (UmiStudioServices *)user_data);
    UmiStatus status;
    (void)argument;
    if (controller == NULL) return UMI_STATUS_INVALID_STATE;
    status = umi_terminal_controller_clear_active(controller);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
                       "%s", status == UMI_STATUS_OK
                           ? "Terminal transcript cleared"
                           : umi_status_text(status));
    }
    return status;
}

static UmiStatus terminal_new_handler(void *user_data,
                                      const char *argument,
                                      char *out_message,
                                      size_t message_capacity)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiTerminalController *controller =
        umi_studio_services_terminal_controller(services);
    UmiTerminalControllerSnapshot controller_snapshot = {0};
    UmiTerminalSessionSnapshot active_snapshot = {0};
    UmiTerminalSession *active;
    char session_id[UMI_TERMINAL_ID_CAPACITY];
    char title[UMI_TERMINAL_TITLE_CAPACITY];
    UmiStatus status;
    if (controller == NULL) return UMI_STATUS_INVALID_STATE;
    active = umi_terminal_controller_active_session(controller);
    if (active == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_terminal_controller_snapshot(controller, &controller_snapshot);
    if (status == UMI_STATUS_OK) {
        status = umi_terminal_session_snapshot(active, &active_snapshot);
    }
    (void)snprintf(session_id, sizeof(session_id), "studio.terminal.%llu",
                   (unsigned long long)(controller_snapshot.revision + 1U));
    (void)snprintf(title, sizeof(title), "Terminal %zu",
                   controller_snapshot.tabs + 1U);
    if (status == UMI_STATUS_OK) {
        status = umi_terminal_controller_open(
            controller,
            argument != NULL ? argument : "",
            session_id,
            title,
            active_snapshot.working_directory);
    }
    if (out_message != NULL && message_capacity > 0U) {
        if (status == UMI_STATUS_OK) {
            (void)snprintf(out_message, message_capacity,
                           "Opened %s", title);
        } else {
            (void)snprintf(out_message, message_capacity,
                           "%s", umi_status_text(status));
        }
    }
    return status;
}

static UmiStatus terminal_close_handler(void *user_data,
                                        const char *argument,
                                        char *out_message,
                                        size_t message_capacity)
{
    UmiStatus status;
    (void)argument;
    status = umi_terminal_controller_close_active(
        umi_studio_services_terminal_controller(
            (UmiStudioServices *)user_data));
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s",
                       status == UMI_STATUS_OK
                           ? "Active terminal closed"
                           : umi_status_text(status));
    }
    return status;
}

static UmiStatus terminal_activate_relative_handler(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity,
    int direction)
{
    UmiStatus status;
    (void)argument;
    status = umi_terminal_controller_activate_relative(
        umi_studio_services_terminal_controller(
            (UmiStudioServices *)user_data), direction);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s",
                       status == UMI_STATUS_OK
                           ? "Active terminal changed"
                           : umi_status_text(status));
    }
    return status;
}

static UmiStatus terminal_next_handler(void *user_data,
                                       const char *argument,
                                       char *out_message,
                                       size_t message_capacity)
{
    return terminal_activate_relative_handler(user_data, argument, out_message,
                                               message_capacity, 1);
}

static UmiStatus terminal_previous_handler(void *user_data,
                                           const char *argument,
                                           char *out_message,
                                           size_t message_capacity)
{
    return terminal_activate_relative_handler(user_data, argument, out_message,
                                               message_capacity, -1);
}

static UmiStatus terminal_split_handler(void *user_data,
                                        const char *argument,
                                        char *out_message,
                                        size_t message_capacity,
                                        UmiTerminalOrientation orientation)
{
    UmiTerminalController *controller = umi_studio_services_terminal_controller(
        (UmiStudioServices *)user_data);
    UmiTerminalControllerSnapshot snapshot = {0};
    char session_id[UMI_TERMINAL_ID_CAPACITY];
    char title[UMI_TERMINAL_TITLE_CAPACITY];
    UmiStatus status;
    (void)argument;
    if (controller == NULL) return UMI_STATUS_INVALID_STATE;
    status = umi_terminal_controller_snapshot(controller, &snapshot);
    (void)snprintf(session_id, sizeof(session_id), "studio.split.%llu",
                   (unsigned long long)(snapshot.revision + 1U));
    (void)snprintf(title, sizeof(title), "Split %zu", snapshot.tabs + 1U);
    if (status == UMI_STATUS_OK) {
        status = umi_terminal_controller_split_active(
            controller, session_id, title, orientation);
    }
    if (out_message != NULL && message_capacity > 0U) {
        if (status == UMI_STATUS_OK) {
            (void)snprintf(out_message, message_capacity,
                           "Created %s", title);
        } else {
            (void)snprintf(out_message, message_capacity,
                           "%s", umi_status_text(status));
        }
    }
    return status;
}

static UmiStatus terminal_split_horizontal_handler(
    void *user_data, const char *argument, char *out_message,
    size_t message_capacity)
{
    return terminal_split_handler(user_data, argument, out_message,
                                  message_capacity,
                                  UMI_TERMINAL_ORIENTATION_HORIZONTAL);
}

static UmiStatus terminal_split_vertical_handler(
    void *user_data, const char *argument, char *out_message,
    size_t message_capacity)
{
    return terminal_split_handler(user_data, argument, out_message,
                                  message_capacity,
                                  UMI_TERMINAL_ORIENTATION_VERTICAL);
}

static UmiStatus terminal_history_clear_handler(
    void *user_data, const char *argument, char *out_message,
    size_t message_capacity)
{
    UmiStatus status;
    (void)argument;
    status = umi_terminal_controller_clear_history(
        umi_studio_services_terminal_controller(
            (UmiStudioServices *)user_data));
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity, "%s",
                       status == UMI_STATUS_OK
                           ? "Terminal history cleared"
                           : umi_status_text(status));
    }
    return status;
}

static UmiStatus terminal_search_handler(void *user_data,
                                         const char *argument,
                                         char *out_message,
                                         size_t message_capacity)
{
    UmiTerminalSearchQuery query;
    UmiTerminalSearchResult result;
    UmiStatus status;
    if (argument == NULL || argument[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_terminal_search_query_init(&query);
    (void)snprintf(query.text, sizeof(query.text), "%s", argument);
    status = umi_terminal_controller_search_active(
        umi_studio_services_terminal_controller(
            (UmiStudioServices *)user_data), &query, &result);
    if (out_message != NULL && message_capacity > 0U) {
        if (status == UMI_STATUS_OK && result.count > 0U) {
            (void)snprintf(out_message, message_capacity,
                           "%zu match(es); first at row %zu, byte %zu: %s",
                           result.total_matches,
                           result.matches[0].line_index + 1U,
                           result.matches[0].byte_offset,
                           result.matches[0].preview);
        } else {
            (void)snprintf(out_message, message_capacity, "%s",
                           status == UMI_STATUS_OK
                               ? "No matches"
                               : umi_status_text(status));
        }
    }
    return status;
}

static UmiStatus process_report_handler(void *user_data,
                                        const char *argument,
                                        char *out_message,
                                        size_t message_capacity)
{
    UmiProcessSupervisorStats stats;
    (void)argument;
    stats = umi_process_supervisor_stats(
        umi_studio_services_process_supervisor(
            (UmiStudioServices *)user_data));
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
                       "Processes: %zu total, %zu running, %llu succeeded, "
                       "%llu failed, %llu cancelled",
                       stats.jobs, stats.running,
                       (unsigned long long)stats.succeeded,
                       (unsigned long long)stats.failed,
                       (unsigned long long)stats.cancelled);
    }
    return UMI_STATUS_OK;
}

static UmiStatus process_cancel_handler(void *user_data,
                                        const char *argument,
                                        char *out_message,
                                        size_t message_capacity)
{
    unsigned long long parsed;
    char *end = NULL;
    UmiStatus status;
    if (argument == NULL || argument[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    errno = 0;
    parsed = strtoull(argument, &end, 10);
    if (errno != 0 || end == argument || *end != '\0' || parsed == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_process_supervisor_cancel(
        umi_studio_services_process_supervisor(
            (UmiStudioServices *)user_data), (UmiProcessJobId)parsed);
    if (out_message != NULL && message_capacity > 0U) {
        if (status == UMI_STATUS_OK) {
            (void)snprintf(out_message, message_capacity,
                           "Cancellation requested for process %llu",
                           parsed);
        } else {
            (void)snprintf(out_message, message_capacity,
                           "Process %llu: %s", parsed,
                           umi_status_text(status));
        }
    }
    return status;
}

static UmiStatus tasks_report_handler(void *user_data,
                                      const char *argument,
                                      char *out_message,
                                      size_t message_capacity)
{
    UmiTaskQueueStats stats;
    (void)argument;
    stats = umi_task_queue_stats(umi_studio_services_task_queue(
        (UmiStudioServices *)user_data));
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
                       "Tasks: %zu queued, %zu running, %llu completed, "
                       "%llu failed, %llu cancelled",
                       stats.queued, stats.running,
                       (unsigned long long)stats.completed,
                       (unsigned long long)stats.failed,
                       (unsigned long long)stats.cancelled);
    }
    return UMI_STATUS_OK;
}

static UmiStatus language_initialize_handler(void *user_data,
                                             const char *argument,
                                             char *out_message,
                                             size_t message_capacity)
{
    int64_t request_id = 0;
    long process_id = 0L;
    char *end = NULL;
    UmiStatus status;
    if (argument != NULL && argument[0] != '\0') {
        process_id = strtol(argument, &end, 10);
        if (end == argument || *end != '\0') return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_studio_language_service_initialize(
        umi_studio_services_language((UmiStudioServices *)user_data),
        (int64_t)process_id,
        &request_id);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "LSP initialise request %lld: %s",
                       (long long)request_id,
                       umi_status_text(status));
    }
    return status;
}

static UmiStatus debug_initialize_handler(void *user_data,
                                          const char *argument,
                                          char *out_message,
                                          size_t message_capacity)
{
    int64_t request_id = 0;
    const char *adapter = argument != NULL && argument[0] != '\0'
        ? argument : "umicom.debug";
    UmiStatus status = umi_studio_debugger_service_initialize(
        umi_studio_services_debugger((UmiStudioServices *)user_data),
        adapter,
        &request_id);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "DAP initialise request %lld: %s",
                       (long long)request_id,
                       umi_status_text(status));
    }
    return status;
}

static UmiStatus language_workspace_symbols_handler(void *user_data,
                                                     const char *argument,
                                                     char *out_message,
                                                     size_t message_capacity)
{
    int64_t request_id = 0;
    UmiStatus status = umi_studio_language_service_workspace_symbols(
        umi_studio_services_language((UmiStudioServices *)user_data),
        argument != NULL ? argument : "", &request_id);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
                       "Workspace-symbol request %lld: %s",
                       (long long)request_id, umi_status_text(status));
    }
    return status;
}

static UmiStatus debug_thread_argument(const char *argument, int *out_thread)
{
    long parsed = 0L;
    char *end = NULL;
    if (out_thread == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (argument != NULL && argument[0] != '\0') {
        parsed = strtol(argument, &end, 10);
        if (end == argument || *end != '\0' || parsed < 0L ||
            parsed > INT32_MAX) return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_thread = (int)parsed;
    return UMI_STATUS_OK;
}

static UmiStatus debug_start_handler(void *user_data, const char *argument,
                                     char *out_message, size_t capacity)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    const UmiBuildProfile *profile = umi_studio_build_service_profile(
        umi_studio_services_build(services));
    const char *adapter = argument != NULL && argument[0] != '\0'
        ? argument : "cppdbg";
    UmiStatus status;
    if (profile == NULL || profile->run_program[0] == '\0') {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_studio_debugger_service_start(
        umi_studio_services_debugger(services), adapter,
        profile->run_program, profile->source_directory);
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "Debug start: %s",
                       umi_status_text(status));
    }
    return status;
}

typedef UmiStatus (*DebugThreadAction)(UmiStudioDebuggerService *, int);
static UmiStatus debug_thread_handler(void *user_data, const char *argument,
                                      char *out_message, size_t capacity,
                                      const char *name,
                                      DebugThreadAction action)
{
    int thread_id = 0;
    UmiStatus status = debug_thread_argument(argument, &thread_id);
    if (status == UMI_STATUS_OK) {
        status = action(umi_studio_services_debugger(
            (UmiStudioServices *)user_data), thread_id);
    }
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s thread %d: %s", name,
                       thread_id, umi_status_text(status));
    }
    return status;
}

#define DEBUG_THREAD_HANDLER(function_name, label, action)                  \
static UmiStatus function_name(void *user_data, const char *argument,       \
                               char *out_message, size_t capacity)          \
{                                                                           \
    return debug_thread_handler(user_data, argument, out_message, capacity, \
                                label, action);                              \
}
DEBUG_THREAD_HANDLER(debug_continue_handler, "Continue", umi_studio_debugger_service_continue)
DEBUG_THREAD_HANDLER(debug_pause_handler, "Pause", umi_studio_debugger_service_pause)
DEBUG_THREAD_HANDLER(debug_next_handler, "Step over", umi_studio_debugger_service_next)
DEBUG_THREAD_HANDLER(debug_step_in_handler, "Step into", umi_studio_debugger_service_step_in)
DEBUG_THREAD_HANDLER(debug_step_out_handler, "Step out", umi_studio_debugger_service_step_out)
#undef DEBUG_THREAD_HANDLER

static UmiStatus debug_stop_handler(void *user_data, const char *argument,
                                    char *out_message, size_t capacity)
{
    UmiStatus status = umi_studio_debugger_service_stop(
        umi_studio_services_debugger((UmiStudioServices *)user_data),
        argument != NULL && strcmp(argument, "restart") == 0);
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "Debug stop: %s",
                       umi_status_text(status));
    }
    return status;
}

static UmiStatus debug_add_breakpoint_handler(void *user_data,
                                              const char *argument,
                                              char *out_message,
                                              size_t capacity)
{
    char path[UMI_PROTOCOL_URI_CAPACITY];
    const char *separator;
    char *end = NULL;
    long line;
    size_t path_length;
    UmiStatus status;
    if (argument == NULL || (separator = strrchr(argument, ':')) == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    path_length = (size_t)(separator - argument);
    if (path_length == 0U || path_length + 1U > sizeof(path)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(path, argument, path_length);
    path[path_length] = '\0';
    line = strtol(separator + 1, &end, 10);
    if (end == separator + 1 || *end != '\0' || line <= 0L ||
        line > INT32_MAX) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_studio_debugger_service_add_breakpoint(
        umi_studio_services_debugger((UmiStudioServices *)user_data),
        path, (int)line, 1);
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "Breakpoint %s:%ld: %s",
                       path, line, umi_status_text(status));
    }
    return status;
}

static UmiStatus debug_set_breakpoint_enabled_handler(
    void *user_data, const char *argument, char *out_message, size_t capacity)
{
    char breakpoint_id[128];
    const char *separator;
    size_t id_length;
    int enabled;
    UmiStatus status;

    if (argument == NULL ||
        (separator = strrchr(argument, '=')) == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    id_length = (size_t)(separator - argument);
    if (id_length == 0U || id_length + 1U > sizeof(breakpoint_id) ||
        (strcmp(separator + 1, "0") != 0 &&
         strcmp(separator + 1, "1") != 0)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memcpy(breakpoint_id, argument, id_length);
    breakpoint_id[id_length] = '\0';
    enabled = strcmp(separator + 1, "1") == 0;
    status = umi_studio_debugger_service_set_breakpoint_enabled(
        umi_studio_services_debugger((UmiStudioServices *)user_data),
        breakpoint_id, enabled);
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "Breakpoint %s %s: %s",
                       breakpoint_id, enabled ? "enabled" : "disabled",
                       umi_status_text(status));
    }
    return status;
}

static UmiStatus debug_remove_breakpoint_handler(
    void *user_data, const char *argument, char *out_message, size_t capacity)
{
    UmiStatus status;
    if (argument == NULL || argument[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_studio_debugger_service_remove_breakpoint(
        umi_studio_services_debugger((UmiStudioServices *)user_data),
        argument);
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "Remove breakpoint %s: %s",
                       argument, umi_status_text(status));
    }
    return status;
}

static UmiStatus debug_add_watch_handler(void *user_data, const char *argument,
                                         char *out_message, size_t capacity)
{
    char watch_id[128] = {0};
    UmiStatus status;
    if (argument == NULL || argument[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_studio_debugger_service_add_watch(
        umi_studio_services_debugger((UmiStudioServices *)user_data),
        argument, watch_id, sizeof(watch_id));
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "Watch %s: %s", watch_id,
                       umi_status_text(status));
    }
    return status;
}

static UmiStatus debug_remove_watch_handler(void *user_data,
                                            const char *argument,
                                            char *out_message,
                                            size_t capacity)
{
    UmiStatus status;
    if (argument == NULL || argument[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_studio_debugger_service_remove_watch(
        umi_studio_services_debugger((UmiStudioServices *)user_data),
        argument);
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "Remove watch %s: %s",
                       argument, umi_status_text(status));
    }
    return status;
}

typedef UmiStatus (*DebugSelectionAction)(UmiStudioDebuggerService *,
                                          const char *);

static UmiStatus debug_selection_handler(void *user_data,
                                         const char *argument,
                                         char *out_message,
                                         size_t capacity,
                                         const char *selection_name,
                                         DebugSelectionAction action)
{
    UmiStatus status;
    if (argument == NULL || argument[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = action(umi_studio_services_debugger(
                        (UmiStudioServices *)user_data), argument);
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "Select %s %s: %s",
                       selection_name, argument, umi_status_text(status));
    }
    return status;
}

#define DEBUG_SELECTION_HANDLER(function_name, label, action)              \
static UmiStatus function_name(void *user_data, const char *argument,       \
                               char *out_message, size_t capacity)          \
{                                                                           \
    return debug_selection_handler(user_data, argument, out_message,        \
                                   capacity, label, action);                \
}
DEBUG_SELECTION_HANDLER(debug_select_thread_handler, "thread",
                        umi_studio_debugger_service_select_thread)
DEBUG_SELECTION_HANDLER(debug_select_frame_handler, "frame",
                        umi_studio_debugger_service_select_frame)
DEBUG_SELECTION_HANDLER(debug_select_scope_handler, "scope",
                        umi_studio_debugger_service_select_scope)
#undef DEBUG_SELECTION_HANDLER

static UmiStatus debug_clear_console_handler(void *user_data,
                                             const char *argument,
                                             char *out_message,
                                             size_t capacity)
{
    UmiStatus status;
    (void)argument;
    status = umi_studio_debugger_service_clear_console(
        umi_studio_services_debugger((UmiStudioServices *)user_data));
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "Debug Console clear: %s",
                       umi_status_text(status));
    }
    return status;
}

static UmiStatus vcs_refresh_handler(void *user_data,
                                     const char *argument,
                                     char *out_message,
                                     size_t message_capacity)
{
    UmiStudioSourceControlSnapshot snapshot;
    UmiStudioSourceControlService *service = umi_studio_services_source_control(
        (UmiStudioServices *)user_data);
    size_t limit = 20U;
    UmiStatus status;
    if (argument != NULL && argument[0] != '\0') {
        char *end = NULL;
        unsigned long parsed = strtoul(argument, &end, 10);
        if (end == argument || *end != '\0' || parsed == 0UL) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        limit = (size_t)parsed;
    }
    status = umi_studio_source_control_service_refresh(service, limit);
    if (status == UMI_STATUS_OK) {
        status = umi_studio_source_control_service_snapshot(service, &snapshot);
    }
    if (out_message != NULL && message_capacity > 0U) {
        if (status == UMI_STATUS_OK) {
            (void)snprintf(out_message,
                           message_capacity,
                           "Git %s: %zu change(s), %zu commit(s)",
                           snapshot.branch,
                           snapshot.changes,
                           snapshot.commits);
        } else {
            (void)snprintf(out_message,
                           message_capacity,
                           "Source control: %s",
                           umi_status_text(status));
        }
    }
    return status;
}

typedef enum StudioVcsCommandOperation {
    STUDIO_VCS_STAGE,
    STUDIO_VCS_UNSTAGE,
    STUDIO_VCS_STAGE_ALL,
    STUDIO_VCS_UNSTAGE_ALL,
    STUDIO_VCS_DISCARD,
    STUDIO_VCS_COMMIT,
    STUDIO_VCS_FETCH,
    STUDIO_VCS_PULL,
    STUDIO_VCS_PUSH,
    STUDIO_VCS_BRANCH_CREATE,
    STUDIO_VCS_BRANCH_CHECKOUT,
    STUDIO_VCS_BRANCH_DELETE,
    STUDIO_VCS_DIFF
} StudioVcsCommandOperation;

static UmiStatus vcs_operation_execute(void *user_data,
                                       const char *argument,
                                       char *out_message,
                                       size_t capacity,
                                       StudioVcsCommandOperation operation)
{
    UmiStudioSourceControlService *service = umi_studio_services_source_control(
        (UmiStudioServices *)user_data);
    UmiStatus status;
    int needs_argument = operation == STUDIO_VCS_STAGE || operation == STUDIO_VCS_UNSTAGE ||
        operation == STUDIO_VCS_DISCARD || operation == STUDIO_VCS_COMMIT ||
        operation == STUDIO_VCS_BRANCH_CREATE || operation == STUDIO_VCS_BRANCH_CHECKOUT ||
        operation == STUDIO_VCS_BRANCH_DELETE || operation == STUDIO_VCS_DIFF;
    if (service == NULL) return UMI_STATUS_UNAVAILABLE;
    if (needs_argument && (argument == NULL || argument[0] == '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    switch (operation) {
        case STUDIO_VCS_STAGE: status = umi_studio_source_control_service_stage(service, argument); break;
        case STUDIO_VCS_UNSTAGE: status = umi_studio_source_control_service_unstage(service, argument); break;
        case STUDIO_VCS_STAGE_ALL: status = umi_studio_source_control_service_stage_all(service); break;
        case STUDIO_VCS_UNSTAGE_ALL: status = umi_studio_source_control_service_unstage_all(service); break;
        case STUDIO_VCS_DISCARD: status = umi_studio_source_control_service_discard(service, argument); break;
        case STUDIO_VCS_COMMIT: status = umi_studio_source_control_service_commit(service, argument, NULL, 0U); break;
        case STUDIO_VCS_FETCH: status = umi_studio_source_control_service_fetch(service); break;
        case STUDIO_VCS_PULL: status = umi_studio_source_control_service_pull(service); break;
        case STUDIO_VCS_PUSH: status = umi_studio_source_control_service_push(service); break;
        case STUDIO_VCS_BRANCH_CREATE: status = umi_studio_source_control_service_branch_create(service, argument, 1); break;
        case STUDIO_VCS_BRANCH_CHECKOUT: status = umi_studio_source_control_service_branch_checkout(service, argument); break;
        case STUDIO_VCS_BRANCH_DELETE: status = umi_studio_source_control_service_branch_delete(service, argument, 0); break;
        case STUDIO_VCS_DIFF: {
            int staged = strncmp(argument, "--staged ", 9U) == 0;
            status = umi_studio_source_control_service_open_diff(service, staged ? argument + 9 : argument, staged);
            break;
        }
        default: status = UMI_STATUS_INVALID_ARGUMENT; break;
    }
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "Source control: %s", umi_status_text(status));
    }
    return status;
}

#define DEFINE_VCS_HANDLER(name_, operation_) \
    static UmiStatus name_(void *user_data, const char *argument, \
                           char *out_message, size_t capacity) \
    { return vcs_operation_execute(user_data, argument, out_message, capacity, operation_); }
DEFINE_VCS_HANDLER(vcs_stage_handler, STUDIO_VCS_STAGE)
DEFINE_VCS_HANDLER(vcs_unstage_handler, STUDIO_VCS_UNSTAGE)
DEFINE_VCS_HANDLER(vcs_stage_all_handler, STUDIO_VCS_STAGE_ALL)
DEFINE_VCS_HANDLER(vcs_unstage_all_handler, STUDIO_VCS_UNSTAGE_ALL)
DEFINE_VCS_HANDLER(vcs_discard_handler, STUDIO_VCS_DISCARD)
DEFINE_VCS_HANDLER(vcs_commit_handler, STUDIO_VCS_COMMIT)
DEFINE_VCS_HANDLER(vcs_fetch_handler, STUDIO_VCS_FETCH)
DEFINE_VCS_HANDLER(vcs_pull_handler, STUDIO_VCS_PULL)
DEFINE_VCS_HANDLER(vcs_push_handler, STUDIO_VCS_PUSH)
DEFINE_VCS_HANDLER(vcs_branch_create_handler, STUDIO_VCS_BRANCH_CREATE)
DEFINE_VCS_HANDLER(vcs_branch_checkout_handler, STUDIO_VCS_BRANCH_CHECKOUT)
DEFINE_VCS_HANDLER(vcs_branch_delete_handler, STUDIO_VCS_BRANCH_DELETE)
DEFINE_VCS_HANDLER(vcs_diff_handler, STUDIO_VCS_DIFF)
#undef DEFINE_VCS_HANDLER

typedef enum StudioVcsWorkspaceOperation {
    STUDIO_VCS_WORKSPACE_FILTER,
    STUDIO_VCS_WORKSPACE_SELECT_CHANGE,
    STUDIO_VCS_WORKSPACE_SELECT_COMMIT,
    STUDIO_VCS_WORKSPACE_SELECT_BRANCH,
    STUDIO_VCS_WORKSPACE_SELECT_REMOTE,
    STUDIO_VCS_WORKSPACE_SET_COMMIT_MESSAGE,
    STUDIO_VCS_WORKSPACE_STAGE_SELECTED,
    STUDIO_VCS_WORKSPACE_UNSTAGE_SELECTED,
    STUDIO_VCS_WORKSPACE_DISCARD_SELECTED,
    STUDIO_VCS_WORKSPACE_COMMIT_COMPOSED,
    STUDIO_VCS_WORKSPACE_DIFF_SELECTED,
    STUDIO_VCS_WORKSPACE_DIFF_SELECTED_STAGED
} StudioVcsWorkspaceOperation;

static UmiVcsWorkspaceCoordinator *vcs_workspace_coordinator(void *user_data)
{
    UmiStudioSourceControlService *service =
        umi_studio_services_source_control((UmiStudioServices *)user_data);
    return service != NULL
        ? umi_studio_source_control_service_coordinator(service) : NULL;
}

static UmiStatus vcs_workspace_operation_execute(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t capacity,
    StudioVcsWorkspaceOperation operation)
{
    UmiVcsWorkspaceCoordinator *coordinator =
        vcs_workspace_coordinator(user_data);
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    int needs_argument =
        operation == STUDIO_VCS_WORKSPACE_FILTER ||
        operation == STUDIO_VCS_WORKSPACE_SELECT_CHANGE ||
        operation == STUDIO_VCS_WORKSPACE_SELECT_COMMIT ||
        operation == STUDIO_VCS_WORKSPACE_SELECT_BRANCH ||
        operation == STUDIO_VCS_WORKSPACE_SELECT_REMOTE ||
        operation == STUDIO_VCS_WORKSPACE_SET_COMMIT_MESSAGE;

    if (coordinator == NULL) return UMI_STATUS_UNAVAILABLE;
    if (needs_argument && (argument == NULL || argument[0] == '\0')) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    switch (operation) {
        case STUDIO_VCS_WORKSPACE_FILTER: {
            UmiVcsChangeFilter filter;
            if (strcmp(argument, "all") == 0 ||
                strcmp(argument, "all changes") == 0) {
                filter = UMI_VCS_CHANGE_FILTER_ALL;
            } else if (strcmp(argument, "staged") == 0) {
                filter = UMI_VCS_CHANGE_FILTER_STAGED;
            } else if (strcmp(argument, "unstaged") == 0) {
                filter = UMI_VCS_CHANGE_FILTER_UNSTAGED;
            } else if (strcmp(argument, "conflicts") == 0) {
                filter = UMI_VCS_CHANGE_FILTER_CONFLICTS;
            } else {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            status = umi_vcs_workspace_coordinator_set_change_filter(
                coordinator, filter);
            break;
        }
        case STUDIO_VCS_WORKSPACE_SELECT_CHANGE:
            status = umi_vcs_workspace_coordinator_select_change(
                coordinator, argument);
            break;
        case STUDIO_VCS_WORKSPACE_SELECT_COMMIT:
            status = umi_vcs_workspace_coordinator_select_commit(
                coordinator, argument);
            break;
        case STUDIO_VCS_WORKSPACE_SELECT_BRANCH:
            status = umi_vcs_workspace_coordinator_select_branch(
                coordinator, argument);
            break;
        case STUDIO_VCS_WORKSPACE_SELECT_REMOTE:
            status = umi_vcs_workspace_coordinator_select_remote(
                coordinator, argument);
            break;
        case STUDIO_VCS_WORKSPACE_SET_COMMIT_MESSAGE:
            status = umi_vcs_workspace_coordinator_set_commit_message(
                coordinator, argument);
            break;
        case STUDIO_VCS_WORKSPACE_STAGE_SELECTED:
            status = umi_vcs_workspace_coordinator_stage_selected(coordinator);
            break;
        case STUDIO_VCS_WORKSPACE_UNSTAGE_SELECTED:
            status = umi_vcs_workspace_coordinator_unstage_selected(
                coordinator);
            break;
        case STUDIO_VCS_WORKSPACE_DISCARD_SELECTED:
            status = umi_vcs_workspace_coordinator_discard_selected(
                coordinator);
            break;
        case STUDIO_VCS_WORKSPACE_COMMIT_COMPOSED:
            status = umi_vcs_workspace_coordinator_commit(
                coordinator, NULL, 0U);
            break;
        case STUDIO_VCS_WORKSPACE_DIFF_SELECTED:
            status = umi_vcs_workspace_coordinator_open_selected_diff(
                coordinator, 0);
            break;
        case STUDIO_VCS_WORKSPACE_DIFF_SELECTED_STAGED:
            status = umi_vcs_workspace_coordinator_open_selected_diff(
                coordinator, 1);
            break;
        default:
            break;
    }
    if (out_message != NULL && capacity > 0U) {
        UmiVcsWorkspaceCoordinatorSnapshot snapshot;
        if (status == UMI_STATUS_OK &&
            umi_vcs_workspace_coordinator_snapshot(
                coordinator, &snapshot) == UMI_STATUS_OK) {
            (void)snprintf(out_message, capacity,
                           "Source control workspace: %s; %zu visible change(s)",
                           snapshot.change_filter_label,
                           snapshot.visible_change_count);
        } else {
            (void)snprintf(out_message, capacity,
                           "Source control workspace: %s",
                           umi_status_text(status));
        }
    }
    return status;
}

#define DEFINE_VCS_WORKSPACE_HANDLER(name_, operation_) \
    static UmiStatus name_(void *user_data, const char *argument, \
                           char *out_message, size_t capacity) \
    { return vcs_workspace_operation_execute(user_data, argument, \
                                              out_message, capacity, \
                                              operation_); }
DEFINE_VCS_WORKSPACE_HANDLER(vcs_filter_handler,
                             STUDIO_VCS_WORKSPACE_FILTER)
DEFINE_VCS_WORKSPACE_HANDLER(vcs_select_change_handler,
                             STUDIO_VCS_WORKSPACE_SELECT_CHANGE)
DEFINE_VCS_WORKSPACE_HANDLER(vcs_select_commit_handler,
                             STUDIO_VCS_WORKSPACE_SELECT_COMMIT)
DEFINE_VCS_WORKSPACE_HANDLER(vcs_select_branch_handler,
                             STUDIO_VCS_WORKSPACE_SELECT_BRANCH)
DEFINE_VCS_WORKSPACE_HANDLER(vcs_select_remote_handler,
                             STUDIO_VCS_WORKSPACE_SELECT_REMOTE)
DEFINE_VCS_WORKSPACE_HANDLER(vcs_set_commit_message_handler,
                             STUDIO_VCS_WORKSPACE_SET_COMMIT_MESSAGE)
DEFINE_VCS_WORKSPACE_HANDLER(vcs_stage_selected_handler,
                             STUDIO_VCS_WORKSPACE_STAGE_SELECTED)
DEFINE_VCS_WORKSPACE_HANDLER(vcs_unstage_selected_handler,
                             STUDIO_VCS_WORKSPACE_UNSTAGE_SELECTED)
DEFINE_VCS_WORKSPACE_HANDLER(vcs_discard_selected_handler,
                             STUDIO_VCS_WORKSPACE_DISCARD_SELECTED)
DEFINE_VCS_WORKSPACE_HANDLER(vcs_commit_composed_handler,
                             STUDIO_VCS_WORKSPACE_COMMIT_COMPOSED)
DEFINE_VCS_WORKSPACE_HANDLER(vcs_diff_selected_handler,
                             STUDIO_VCS_WORKSPACE_DIFF_SELECTED)
DEFINE_VCS_WORKSPACE_HANDLER(vcs_diff_selected_staged_handler,
                             STUDIO_VCS_WORKSPACE_DIFF_SELECTED_STAGED)
#undef DEFINE_VCS_WORKSPACE_HANDLER

/* Trading commands operate only on the Framework workspace owned by Studio.
 * There is deliberately no broker adapter or command that arms live trading. */
static UmiTradingWorkspace *trading_workspace(void *user_data)
{
    UmiStudioTradingService *service = umi_studio_services_trading(
        (UmiStudioServices *)user_data);
    return umi_studio_trading_service_workspace(service);
}

static int has_argument(const char *argument)
{
    return argument != NULL && argument[0] != '\0';
}

static UmiStatus parse_real_argument(const char *argument, double *out_value)
{
    char *end = NULL;
    double value;

    if (!has_argument(argument) || out_value == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    errno = 0;
    value = strtod(argument, &end);
    if (errno != 0 || end == argument || *end != '\0' || !isfinite(value))
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_value = value;
    return UMI_STATUS_OK;
}

static void trading_message(char *out_message, size_t message_capacity,
                            const char *success_text, UmiStatus status)
{
    if (out_message == NULL || message_capacity == 0U) return;
    (void)snprintf(out_message, message_capacity, "%s",
        status == UMI_STATUS_OK ? success_text : umi_status_text(status));
}

static UmiStatus trading_refresh_handler(void *user_data,
                                         const char *argument,
                                         char *out_message,
                                         size_t message_capacity)
{
    UmiStatus status;
    (void)argument;
    status = umi_trading_workspace_refresh(trading_workspace(user_data));
    trading_message(out_message, message_capacity,
                    "Trading workspace refreshed", status);
    return status;
}

static UmiStatus trading_filter_instruments_handler(
    void *user_data, const char *argument,
    char *out_message, size_t message_capacity)
{
    UmiStatus status = umi_trading_workspace_set_instrument_filter(
        trading_workspace(user_data), argument != NULL ? argument : "");
    trading_message(out_message, message_capacity,
                    "Trading instrument filter updated", status);
    return status;
}

static UmiStatus trading_select_instrument_handler(
    void *user_data, const char *argument,
    char *out_message, size_t message_capacity)
{
    UmiStatus status = has_argument(argument)
        ? umi_trading_workspace_select_instrument(
              trading_workspace(user_data), argument)
        : UMI_STATUS_INVALID_ARGUMENT;
    trading_message(out_message, message_capacity,
                    "Trading instrument selected", status);
    return status;
}

static UmiStatus trading_set_environment_handler(
    void *user_data, const char *argument,
    char *out_message, size_t message_capacity)
{
    UmiTradingEnvironment environment;
    UmiStatus status;

    if (argument != NULL && strcmp(argument, "simulation") == 0)
        environment = UMI_TRADING_SIMULATION;
    else if (argument != NULL && strcmp(argument, "paper") == 0)
        environment = UMI_TRADING_PAPER;
    else if (argument != NULL && strcmp(argument, "live") == 0)
        environment = UMI_TRADING_LIVE;
    else
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_workspace_set_environment(
        trading_workspace(user_data), environment);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
            status == UMI_STATUS_OK
                ? "Trading environment set to %s; safety gates remain active"
                : "Trading environment: %s",
            status == UMI_STATUS_OK ? umi_trading_environment_text(environment)
                                    : umi_status_text(status));
    }
    return status;
}

static UmiStatus trading_set_side_handler(void *user_data,
                                          const char *argument,
                                          char *out_message,
                                          size_t message_capacity)
{
    UmiSide side;
    UmiStatus status;

    if (argument != NULL && strcmp(argument, "buy") == 0)
        side = UMI_SIDE_BUY;
    else if (argument != NULL && strcmp(argument, "sell") == 0)
        side = UMI_SIDE_SELL;
    else
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_workspace_set_draft_side(
        trading_workspace(user_data), side);
    trading_message(out_message, message_capacity,
                    "Draft order side updated", status);
    return status;
}

static UmiStatus parse_order_type_argument(const char *argument,
                                           UmiOrderType *out_type,
                                           UmiTimeInForce *out_tif)
{
    char buffer[64];
    char *separator;
    const char *tif_text;
    size_t length;

    if (!has_argument(argument) || out_type == NULL || out_tif == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(argument);
    if (length >= sizeof(buffer)) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(buffer, argument, length + 1U);
    separator = strchr(buffer, ':');
    if (separator != NULL) {
        *separator = '\0';
        tif_text = separator + 1;
    } else {
        tif_text = "day";
    }
    if (strcmp(buffer, "market") == 0) *out_type = UMI_ORDER_MARKET;
    else if (strcmp(buffer, "limit") == 0) *out_type = UMI_ORDER_LIMIT;
    else if (strcmp(buffer, "stop") == 0) *out_type = UMI_ORDER_STOP;
    else if (strcmp(buffer, "stop-limit") == 0)
        *out_type = UMI_ORDER_STOP_LIMIT;
    else return UMI_STATUS_INVALID_ARGUMENT;

    if (strcmp(tif_text, "day") == 0) *out_tif = UMI_TIF_DAY;
    else if (strcmp(tif_text, "gtc") == 0) *out_tif = UMI_TIF_GTC;
    else if (strcmp(tif_text, "ioc") == 0) *out_tif = UMI_TIF_IOC;
    else if (strcmp(tif_text, "fok") == 0) *out_tif = UMI_TIF_FOK;
    else return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

static UmiStatus trading_set_type_handler(void *user_data,
                                          const char *argument,
                                          char *out_message,
                                          size_t message_capacity)
{
    UmiOrderType type;
    UmiTimeInForce tif;
    UmiStatus status = parse_order_type_argument(argument, &type, &tif);
    if (status == UMI_STATUS_OK) {
        status = umi_trading_workspace_set_draft_type(
            trading_workspace(user_data), type, tif);
    }
    trading_message(out_message, message_capacity,
                    "Draft order type updated", status);
    return status;
}

static UmiStatus trading_set_quantity_handler(
    void *user_data, const char *argument,
    char *out_message, size_t message_capacity)
{
    double quantity = 0.0;
    UmiStatus status = parse_real_argument(argument, &quantity);
    if (status == UMI_STATUS_OK) {
        status = umi_trading_workspace_set_draft_quantity(
            trading_workspace(user_data), quantity);
    }
    trading_message(out_message, message_capacity,
                    "Draft order quantity updated", status);
    return status;
}

static UmiStatus trading_set_prices_handler(void *user_data,
                                            const char *argument,
                                            char *out_message,
                                            size_t message_capacity)
{
    char buffer[96];
    char *separator;
    double limit_price = 0.0;
    double stop_price = 0.0;
    size_t length;
    UmiStatus status;

    if (!has_argument(argument)) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(argument);
    if (length >= sizeof(buffer)) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(buffer, argument, length + 1U);
    separator = strchr(buffer, ':');
    if (separator != NULL) *separator = '\0';
    status = parse_real_argument(buffer, &limit_price);
    if (status == UMI_STATUS_OK && separator != NULL)
        status = parse_real_argument(separator + 1, &stop_price);
    if (status == UMI_STATUS_OK) {
        status = umi_trading_workspace_set_draft_prices(
            trading_workspace(user_data), limit_price, stop_price);
    }
    trading_message(out_message, message_capacity,
                    "Draft order prices updated", status);
    return status;
}

static UmiStatus trading_preview_order_handler(
    void *user_data, const char *argument,
    char *out_message, size_t message_capacity)
{
    UmiRiskDecision decision = {0};
    UmiStatus status;
    (void)argument;
    status = umi_trading_workspace_preview_order(
        trading_workspace(user_data), &decision);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
            "Pre-trade risk: %s%s%s",
            decision.allowed ? "allowed" : "denied",
            decision.reason[0] != '\0' ? " — " : "",
            decision.reason);
    }
    return status;
}

static UmiStatus trading_submit_order_handler(void *user_data,
                                              const char *argument,
                                              char *out_message,
                                              size_t message_capacity)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiClock *clock = umi_studio_services_clock(services);
    UmiTradingWorkspace *workspace = trading_workspace(user_data);
    UmiTradingWorkspaceSnapshot snapshot;
    UmiRiskDecision decision = {0};
    uint64_t now_ns;
    int64_t now_ms;
    UmiStatus status;
    (void)argument;

    now_ns = clock->wall_nanoseconds(clock);
    if (now_ns / UINT64_C(1000000) > (uint64_t)INT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    now_ms = (int64_t)(now_ns / UINT64_C(1000000));
    status = umi_trading_workspace_submit_order(workspace, now_ms, &decision);
    if (out_message != NULL && message_capacity > 0U) {
        if (status == UMI_STATUS_OK &&
            umi_trading_workspace_snapshot(workspace, &snapshot) ==
                UMI_STATUS_OK) {
            (void)snprintf(out_message, message_capacity,
                "Submitted simulation order %s",
                snapshot.selected_order_id);
        } else {
            (void)snprintf(out_message, message_capacity,
                "Order not submitted: %s",
                decision.reason[0] != '\0' ? decision.reason
                                            : umi_status_text(status));
        }
    }
    return status;
}

static UmiStatus trading_filter_orders_handler(
    void *user_data, const char *argument,
    char *out_message, size_t message_capacity)
{
    UmiTradingWorkspaceOrderFilter filter;
    UmiStatus status;

    if (argument != NULL && strcmp(argument, "all") == 0)
        filter = UMI_TRADING_WORKSPACE_ORDERS_ALL;
    else if (argument != NULL && strcmp(argument, "open") == 0)
        filter = UMI_TRADING_WORKSPACE_ORDERS_OPEN;
    else if (argument != NULL && strcmp(argument, "filled") == 0)
        filter = UMI_TRADING_WORKSPACE_ORDERS_FILLED;
    else if (argument != NULL && strcmp(argument, "cancelled") == 0)
        filter = UMI_TRADING_WORKSPACE_ORDERS_CANCELLED;
    else if (argument != NULL && strcmp(argument, "rejected") == 0)
        filter = UMI_TRADING_WORKSPACE_ORDERS_REJECTED;
    else
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_workspace_set_order_filter(
        trading_workspace(user_data), filter);
    trading_message(out_message, message_capacity,
                    "Trading order filter updated", status);
    return status;
}

static UmiStatus trading_select_order_handler(
    void *user_data, const char *argument,
    char *out_message, size_t message_capacity)
{
    UmiStatus status = has_argument(argument)
        ? umi_trading_workspace_select_order(
              trading_workspace(user_data), argument)
        : UMI_STATUS_INVALID_ARGUMENT;
    trading_message(out_message, message_capacity,
                    "Trading order selected", status);
    return status;
}

static UmiStatus trading_cancel_order_handler(
    void *user_data, const char *argument,
    char *out_message, size_t message_capacity)
{
    UmiStatus status;
    (void)argument;
    status = umi_trading_workspace_cancel_selected_order(
        trading_workspace(user_data));
    trading_message(out_message, message_capacity,
                    "Selected trading order cancelled", status);
    return status;
}

static UmiStatus trading_engage_kill_switch_handler(
    void *user_data, const char *argument,
    char *out_message, size_t message_capacity)
{
    umi_trading_workspace_engage_kill_switch(
        trading_workspace(user_data),
        has_argument(argument) ? argument : "Studio operator request");
    trading_message(out_message, message_capacity,
                    "Trading kill switch engaged", UMI_STATUS_OK);
    return UMI_STATUS_OK;
}

static UmiStatus trading_reset_kill_switch_handler(
    void *user_data, const char *argument,
    char *out_message, size_t message_capacity)
{
    (void)argument;
    umi_trading_workspace_reset_kill_switch(trading_workspace(user_data));
    trading_message(out_message, message_capacity,
                    "Trading kill switch reset", UMI_STATUS_OK);
    return UMI_STATUS_OK;
}

static UmiStatus developer_report_handler(void *user_data,
                                          const char *argument,
                                          char *out_message,
                                          size_t message_capacity)
{
    (void)argument;
    return umi_studio_developer_platform_report(
        umi_studio_services_developer_platform(
            (UmiStudioServices *)user_data),
        out_message,
        message_capacity);
}

static UmiStatus ai_refresh_health_handler(void *user_data,
                                           const char *argument,
                                           char *out_message,
                                           size_t message_capacity)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiClock *clock = umi_studio_services_clock(services);
    size_t healthy = 0U;
    UmiStatus status;
    (void)argument;
    status = umi_studio_ai_platform_refresh_health(
        umi_studio_services_ai_platform(services),
        clock->wall_nanoseconds(clock), &healthy);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
                       "AuthorEngine health refreshed: %zu healthy runtime(s)",
                       healthy);
    }
    return status;
}

static UmiStatus ai_new_session_handler(void *user_data,
                                        const char *argument,
                                        char *out_message,
                                        size_t message_capacity)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiClock *clock = umi_studio_services_clock(services);
    uint64_t now = clock->wall_nanoseconds(clock);
    char generated_id[UMI_AI_ID_CAPACITY];
    const char *session_id = argument;
    UmiStatus status;
    if (session_id == NULL || session_id[0] == '\0') {
        (void)snprintf(generated_id, sizeof(generated_id),
                       "studio.session.%" PRIu64, now);
        session_id = generated_id;
    }
    status = umi_studio_ai_platform_begin_session(
        umi_studio_services_ai_platform(services), session_id,
        "Studio AI conversation", now);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
                       status == UMI_STATUS_OK
                           ? "Created AI session %s" : "AI session: %s",
                       status == UMI_STATUS_OK ? session_id
                                               : umi_status_text(status));
    }
    return status;
}

static UmiStatus ai_save_session_handler(void *user_data,
                                         const char *argument,
                                         char *out_message,
                                         size_t message_capacity)
{
    UmiStudioAiPlatform *platform = umi_studio_services_ai_platform(
        (UmiStudioServices *)user_data);
    UmiAiAuthorEngineServiceSnapshot snapshot;
    UmiStatus status;
    if (argument == NULL || argument[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_studio_ai_platform_snapshot(platform, &snapshot);
    if (status == UMI_STATUS_OK && snapshot.active_session_id[0] == '\0') {
        status = UMI_STATUS_INVALID_STATE;
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_ai_platform_save_session(
            platform, snapshot.active_session_id, argument);
    }
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
                       status == UMI_STATUS_OK
                           ? "Saved AI session to %s" : "AI session save: %s",
                       status == UMI_STATUS_OK ? argument
                                               : umi_status_text(status));
    }
    return status;
}

static UmiStatus coding_task_execute(void *user_data,
                                     const char *argument,
                                     char *out_message,
                                     size_t message_capacity,
                                     UmiAiCodingTaskKind task)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiStudioAiPlatform *platform = umi_studio_services_ai_platform(services);
    UmiClock *clock = umi_studio_services_clock(services);
    UmiAiCodingTaskPlan plan;
    char request_id[UMI_AI_ID_CAPACITY];
    char active_path[UMI_AI_TEXT_CAPACITY];
    char instruction[UMI_AI_TEXT_CAPACITY];
    const char *separator = argument != NULL ? strchr(argument, '|') : NULL;
    UmiStatus status;

    (void)snprintf(request_id, sizeof(request_id), "studio.coding.%u.%" PRIu64,
                   (unsigned int)task, clock->wall_nanoseconds(clock));
    (void)snprintf(active_path, sizeof(active_path), "%s",
                   "applications/studio/src/app/ai_platform.c");
    (void)snprintf(instruction, sizeof(instruction), "%s",
                   argument != NULL && argument[0] != '\0'
                       ? argument : "Work with the active code selection.");
    /* A frontend may send relative-path|instruction.  Keeping the transport
     * textual makes the command available to palettes, consoles and tests. */
    if (separator != NULL) {
        size_t path_length = (size_t)(separator - argument);
        if (path_length == 0U || path_length >= sizeof(active_path)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        (void)memcpy(active_path, argument, path_length);
        active_path[path_length] = '\0';
        (void)snprintf(instruction, sizeof(instruction), "%s", separator + 1);
    }
    status = umi_studio_coding_assistant_prepare(
        platform, request_id, task, instruction, active_path, &plan);
    if (out_message != NULL && message_capacity > 0U) {
        if (status == UMI_STATUS_OK) {
            (void)snprintf(
                out_message, message_capacity,
                "%s plan %s: %zu repository file(s), %" PRIu32
                " total context tokens, review hash %016" PRIx64,
                umi_ai_coding_task_kind_text(task), request_id,
                plan.repository_context.file_count, plan.total_context_tokens,
                plan.plan_hash);
        } else {
            (void)snprintf(out_message, message_capacity,
                           "AI coding task: %s", umi_status_text(status));
        }
    }
    return status;
}

#define DEFINE_CODING_TASK_HANDLER(name_, task_) \
    static UmiStatus name_(void *user_data, const char *argument, \
                           char *out_message, size_t capacity) \
    { \
        return coding_task_execute( \
            user_data, argument, out_message, capacity, task_); \
    }
DEFINE_CODING_TASK_HANDLER(ai_code_chat_handler, UMI_AI_CODING_TASK_CHAT)
DEFINE_CODING_TASK_HANDLER(ai_complete_code_handler,
                           UMI_AI_CODING_TASK_COMPLETE)
DEFINE_CODING_TASK_HANDLER(ai_explain_code_handler,
                           UMI_AI_CODING_TASK_EXPLAIN)
DEFINE_CODING_TASK_HANDLER(ai_refactor_code_handler,
                           UMI_AI_CODING_TASK_REFACTOR)
DEFINE_CODING_TASK_HANDLER(ai_generate_tests_handler,
                           UMI_AI_CODING_TASK_GENERATE_TESTS)
#undef DEFINE_CODING_TASK_HANDLER

static UmiStatus resolve_patch_id(UmiStudioAiPlatform *platform,
                                  const char *argument,
                                  char *out_patch_id,
                                  size_t capacity)
{
    UmiAiCodingAssistantSnapshot snapshot;
    int written;
    if (argument != NULL && argument[0] != '\0') {
        written = snprintf(out_patch_id, capacity, "%s", argument);
        return written >= 0 && (size_t)written < capacity
            ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (umi_ai_coding_assistant_snapshot(
            umi_studio_ai_platform_coding_assistant(platform), &snapshot)
        != UMI_STATUS_OK || snapshot.last_patch_id[0] == '\0') {
        return UMI_STATUS_NOT_FOUND;
    }
    written = snprintf(out_patch_id, capacity, "%s", snapshot.last_patch_id);
    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

static UmiStatus ai_patch_approve_handler(void *user_data,
                                          const char *argument,
                                          char *out_message,
                                          size_t message_capacity)
{
    UmiStudioAiPlatform *platform = umi_studio_services_ai_platform(
        (UmiStudioServices *)user_data);
    char patch_id[UMI_AI_ID_CAPACITY];
    UmiStatus status = resolve_patch_id(
        platform, argument, patch_id, sizeof(patch_id));
    if (status == UMI_STATUS_OK) {
        status = umi_studio_coding_assistant_approve_patch(
            platform, patch_id, "studio.user");
    }
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
                       status == UMI_STATUS_OK ? "Approved AI patch %s"
                                               : "AI patch approval: %s",
                       status == UMI_STATUS_OK ? patch_id
                                               : umi_status_text(status));
    }
    return status;
}

static UmiStatus coding_patch_mutate(void *user_data,
                                     const char *argument,
                                     char *out_message,
                                     size_t message_capacity,
                                     int revert)
{
    UmiStudioAiPlatform *platform = umi_studio_services_ai_platform(
        (UmiStudioServices *)user_data);
    UmiAiAuthorEngineServiceSnapshot integration;
    UmiStudioCodingWorkspace workspace;
    UmiAiCodingFileAdapter adapter;
    char patch_id[UMI_AI_ID_CAPACITY];
    UmiStatus status = resolve_patch_id(
        platform, argument, patch_id, sizeof(patch_id));
    if (status == UMI_STATUS_OK) {
        status = umi_studio_ai_platform_snapshot(platform, &integration);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_coding_workspace_adapter_init(
            &workspace, integration.workspace, &adapter);
    }
    if (status == UMI_STATUS_OK) {
        status = revert
            ? umi_studio_coding_assistant_revert_patch(
                  platform, patch_id, &adapter)
            : umi_studio_coding_assistant_apply_patch(
                  platform, patch_id, &adapter);
    }
    if (out_message != NULL && message_capacity > 0U) {
        if (status == UMI_STATUS_OK) {
            (void)snprintf(out_message, message_capacity, "%s AI patch %s",
                           revert ? "Reverted" : "Applied", patch_id);
        } else {
            (void)snprintf(out_message, message_capacity, "AI patch %s: %s",
                           revert ? "revert" : "apply",
                           umi_status_text(status));
        }
    }
    return status;
}

static UmiStatus ai_patch_apply_handler(void *user_data,
                                        const char *argument,
                                        char *out_message,
                                        size_t message_capacity)
{
    return coding_patch_mutate(
        user_data, argument, out_message, message_capacity, 0);
}

static UmiStatus ai_patch_revert_handler(void *user_data,
                                         const char *argument,
                                         char *out_message,
                                         size_t message_capacity)
{
    return coding_patch_mutate(
        user_data, argument, out_message, message_capacity, 1);
}

static UmiStatus register_command(UmiCommandRegistry *registry,
                                  UmiStudioServices *services,
                                  const char *command_id,
                                  const char *title,
                                  const char *category,
                                  const char *description,
                                  const char *permission,
                                  uint32_t flags,
                                  UmiRegisteredCommandHandler handler)
{
    UmiCommandDescriptor descriptor;

    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.command_id = command_id;
    descriptor.title = title;
    descriptor.category = category;
    descriptor.description = description;
    descriptor.required_permission = permission;
    descriptor.flags = flags;
    descriptor.handler = handler;
    descriptor.enabled = NULL;
    descriptor.user_data = services;
    return umi_command_registry_register(registry, &descriptor);
}

UmiStatus umi_studio_commands_register(UmiCommandRegistry *registry,
                                       UmiStudioServices *services)
{
    UmiStatus status;

    if (registry == NULL || services == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = register_command(registry,
                              services,
                              UMI_STUDIO_COMMAND_SESSION_SAVE,
                              "Save session",
                              "Session",
                              "Persist the current Studio session.",
                              "studio.session.write",
                              UMI_COMMAND_MUTATES_STATE |
                                  UMI_COMMAND_AUDITED,
                              session_save_handler);
    if (status != UMI_STATUS_OK) return status;

    status = register_command(registry,
                              services,
                              UMI_STUDIO_COMMAND_DOCUMENTS_SAVE_ALL,
                              "Save all documents",
                              "Documents",
                              "Save every modified document that has a path.",
                              "studio.documents.write",
                              UMI_COMMAND_MUTATES_STATE |
                                  UMI_COMMAND_AUDITED,
                              documents_save_all_handler);
    if (status != UMI_STATUS_OK) return status;

    status = register_command(registry,
                              services,
                              UMI_STUDIO_COMMAND_TASKS_WAIT_IDLE,
                              "Wait for tasks",
                              "Tasks",
                              "Wait for the shared Studio task queue to become idle.",
                              "studio.tasks.read",
                              UMI_COMMAND_NONE,
                              tasks_wait_idle_handler);
    if (status != UMI_STATUS_OK) return status;

    status = register_command(registry,
                              services,
                              UMI_STUDIO_COMMAND_TASKS_REPORT,
                              "Report Tasks",
                              "Tasks",
                              "Report shared task queue progress and totals.",
                              "studio.tasks.read",
                              UMI_COMMAND_NONE,
                              tasks_report_handler);
    if (status != UMI_STATUS_OK) return status;

    status = register_command(registry,
                              services,
                              UMI_STUDIO_COMMAND_RECOVERY_PURGE,
                              "Purge recovery data",
                              "Recovery",
                              "Remove all Studio crash-recovery records.",
                              "studio.recovery.delete",
                              UMI_COMMAND_MUTATES_STATE |
                                  UMI_COMMAND_AUDITED |
                                  UMI_COMMAND_REQUIRES_TRUST,
                              recovery_purge_handler);
    if (status != UMI_STATUS_OK) return status;

    status = register_command(registry,
                              services,
                              UMI_STUDIO_COMMAND_WORKSPACE_REFRESH,
                              "Refresh workspace",
                              "Workspace",
                              "Rediscover projects and rebuild the file index.",
                              "studio.workspace.read",
                              UMI_COMMAND_NONE,
                              workspace_refresh_handler);
    if (status != UMI_STATUS_OK) return status;

    status = register_command(registry,
                              services,
                              UMI_STUDIO_COMMAND_WORKSPACE_CLOSE,
                              "Close workspace",
                              "Workspace",
                              "Stop watching and close the active workspace.",
                              "studio.workspace.write",
                              UMI_COMMAND_MUTATES_STATE,
                              workspace_close_handler);
    if (status != UMI_STATUS_OK) return status;

    status = register_command(registry,
                              services,
                              UMI_STUDIO_COMMAND_WATCHER_SCAN,
                              "Scan workspace",
                              "Workspace",
                              "Run one deterministic workspace watcher scan.",
                              "studio.workspace.read",
                              UMI_COMMAND_NONE,
                              watcher_scan_handler);
    if (status != UMI_STATUS_OK) return status;

    status = register_command(registry,
                              services,
                              UMI_STUDIO_COMMAND_DATA_INTEGRITY,
                              "Check Data Server integrity",
                              "Data",
                              "Verify the authoritative Studio Data Server.",
                              "data.read",
                              UMI_COMMAND_NONE,
                              data_integrity_handler);
    if (status != UMI_STATUS_OK) return status;

    status = register_command(registry,
                              services,
                              UMI_STUDIO_COMMAND_MESSAGES_FLUSH,
                              "Flush message outbox",
                              "Messaging",
                              "Deliver pending Studio Integration Fabric records.",
                              "messaging.publish",
                              UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED,
                              messages_flush_handler);
    if (status != UMI_STATUS_OK) return status;

    status = register_command(registry,
                              services,
                              UMI_STUDIO_COMMAND_MESSAGES_REPLAY,
                              "Replay durable messages",
                              "Messaging",
                              "Replay Studio journal records through the dispatcher.",
                              "messaging.replay",
                              UMI_COMMAND_AUDITED,
                              messages_replay_handler);
    if (status != UMI_STATUS_OK) return status;

    status = register_command(registry,
                              services,
                              UMI_STUDIO_COMMAND_SECURITY_REPORT,
                              "Security report",
                              "Security",
                              "Inspect Studio identities, roles, trust and policy evidence.",
                              "studio.security.read",
                              UMI_COMMAND_AUDITED,
                              security_report_handler);
    if (status != UMI_STATUS_OK) return status;

    status = register_command(registry,
                              services,
                              UMI_STUDIO_COMMAND_PLUGINS_REPORT,
                              "Plug-in report",
                              "Plug-ins",
                              "Inspect the Studio plug-in catalogue and contributions.",
                              "studio.plugins.read",
                              UMI_COMMAND_NONE,
                              plugins_report_handler);
    if (status != UMI_STATUS_OK) return status;

    status = register_command(registry,
                              services,
                              UMI_STUDIO_COMMAND_MARKETPLACE_CHECK_UPDATES,
                              "Check for product updates",
                              "Products",
                              "Evaluate installed products against trusted and verified releases.",
                              "studio.products.read",
                              UMI_COMMAND_AUDITED,
                              marketplace_check_updates_handler);
    if (status != UMI_STATUS_OK) return status;

    status = register_command(registry,
                              services,
                              UMI_STUDIO_COMMAND_MARKETPLACE_PLAN_UPDATE,
                              "Plan product update",
                              "Products",
                              "Create a recoverable update transaction for a release ID.",
                              "studio.products.manage",
                              UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED,
                              marketplace_plan_update_handler);
    if (status != UMI_STATUS_OK) return status;

    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_AI_REFRESH_HEALTH,
                              "Refresh AuthorEngine health", "AI",
                              "Probe registered AI providers and configured AuthorEngine runtimes.",
                              "studio.ai.read", UMI_COMMAND_AUDITED,
                              ai_refresh_health_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_AI_NEW_SESSION,
                              "New AI session", "AI",
                              "Create a governed AuthorEngine conversation session.",
                              "studio.ai.manage",
                              UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED,
                              ai_new_session_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_AI_SAVE_SESSION,
                              "Save AI session", "AI",
                              "Persist the active conversation when privacy policy permits.",
                              "studio.ai.manage",
                              UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED,
                              ai_save_session_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_AI_CODE_CHAT,
                              "Code chat", "AI Coding",
                              "Plan a repository-aware coding conversation.",
                              "studio.ai.read", UMI_COMMAND_AUDITED,
                              ai_code_chat_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_AI_COMPLETE_CODE,
                              "Complete code", "AI Coding",
                              "Plan context-aware code completion for the active file.",
                              "studio.ai.read", UMI_COMMAND_AUDITED,
                              ai_complete_code_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_AI_EXPLAIN_CODE,
                              "Explain code", "AI Coding",
                              "Plan an explanation using governed repository context.",
                              "studio.ai.read", UMI_COMMAND_AUDITED,
                              ai_explain_code_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_AI_REFACTOR_CODE,
                              "Refactor code", "AI Coding",
                              "Plan a refactoring whose patch requires review.",
                              "studio.ai.manage", UMI_COMMAND_AUDITED,
                              ai_refactor_code_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_AI_GENERATE_TESTS,
                              "Generate tests", "AI Coding",
                              "Plan repository-aware tests as a reviewable patch.",
                              "studio.ai.manage", UMI_COMMAND_AUDITED,
                              ai_generate_tests_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_AI_PATCH_APPROVE,
                              "Approve AI patch", "AI Coding",
                              "Approve the exact current AI patch preview.",
                              "studio.ai.manage",
                              UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED |
                                  UMI_COMMAND_REQUIRES_TRUST,
                              ai_patch_approve_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_AI_PATCH_APPLY,
                              "Apply AI patch", "AI Coding",
                              "Apply an approved patch after workspace conflict checks.",
                              "studio.documents.write",
                              UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED |
                                  UMI_COMMAND_REQUIRES_TRUST,
                              ai_patch_apply_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_AI_PATCH_REVERT,
                              "Revert AI patch", "AI Coding",
                              "Revert an applied patch when its files are unchanged.",
                              "studio.documents.write",
                              UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED |
                                  UMI_COMMAND_REQUIRES_TRUST,
                              ai_patch_revert_handler);
    if (status != UMI_STATUS_OK) return status;

    status = register_command(registry,
                              services,
                              UMI_STUDIO_COMMAND_OBSERVABILITY_REPORT,
                              "Observability report",
                              "Operations",
                              "Inspect metrics, traces, audit, readiness and events.",
                              "studio.observability.read",
                              UMI_COMMAND_NONE,
                              observability_report_handler);
    if (status != UMI_STATUS_OK) return status;

    status = register_command(registry,
                              services,
                              UMI_STUDIO_COMMAND_RESILIENCE_REPORT,
                              "Resilience report",
                              "Operations",
                              "Inspect supervised components, circuits and rate limits.",
                              "studio.resilience.read",
                              UMI_COMMAND_NONE,
                              resilience_report_handler);
    if (status != UMI_STATUS_OK) return status;

    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_BUILD_CONFIGURE,
                              "Configure", "Build",
                              "Configure the active Studio build profile.",
                              "studio.build.execute",
                              UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED,
                              build_configure_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_BUILD_COMPILE,
                              "Build", "Build",
                              "Compile the active Studio build profile.",
                              "studio.build.execute",
                              UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED,
                              build_compile_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_BUILD_TEST,
                              "Test", "Build",
                              "Run CTest for the active build profile.",
                              "studio.tests.execute",
                              UMI_COMMAND_AUDITED,
                              build_test_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_BUILD_CLEAN,
                              "Clean", "Build",
                              "Clean the active build profile.",
                              "studio.build.execute",
                              UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED,
                              build_clean_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_BUILD_RUN,
                              "Start Studio", "Run",
                              "Start the configured Umicom Studio executable.",
                              "process.execute",
                              UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED,
                              build_run_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_BUILD_INSTALL,
                              "Deploy Local", "Deploy",
                              "Install the active profile into its local staging prefix.",
                              "studio.delivery.execute",
                              UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED,
                              build_install_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_BUILD_RETRY,
                              "Retry Operation", "Build",
                              "Retry a failed dependency-graph node.",
                              "studio.build.execute",
                              UMI_COMMAND_MUTATES_STATE |
                                  UMI_COMMAND_REQUIRES_TRUST |
                                  UMI_COMMAND_AUDITED,
                              build_retry_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_BUILD_CANCEL,
                              "Cancel Build", "Build",
                              "Request cancellation of the active build operation.",
                              "studio.build.execute",
                              UMI_COMMAND_MUTATES_STATE |
                                  UMI_COMMAND_REQUIRES_TRUST |
                                  UMI_COMMAND_AUDITED,
                              build_cancel_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_BUILD_FILTER,
                              "Filter Build Graph", "Build",
                              "Filter build nodes by text or lifecycle state.",
                              "studio.build.read", UMI_COMMAND_NONE,
                              build_filter_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_BUILD_SELECT_NODE,
                              "Select Build Node", "Build",
                              "Select a build graph node by stable identifier.",
                              "studio.build.read", UMI_COMMAND_NONE,
                              build_select_node_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_BUILD_SELECT_OPERATION,
                              "Select Build Operation", "Build",
                              "Select retained build evidence by operation ID.",
                              "studio.build.read", UMI_COMMAND_NONE,
                              build_select_operation_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_BUILD_SELECT_ARTIFACT,
                              "Select Build Artifact", "Build",
                              "Select a produced artifact by stable identifier.",
                              "studio.build.read", UMI_COMMAND_NONE,
                              build_select_artifact_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_BUILD_RUN_NEXT,
                              "Run Next Build Node", "Build",
                              "Execute the next ready dependency-graph node.",
                              "studio.build.execute",
                              UMI_COMMAND_MUTATES_STATE |
                                  UMI_COMMAND_REQUIRES_TRUST |
                                  UMI_COMMAND_AUDITED,
                              build_run_next_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_BUILD_RUN_ALL,
                              "Run Build Plan", "Build",
                              "Execute ready build nodes in dependency order.",
                              "studio.build.execute",
                              UMI_COMMAND_MUTATES_STATE |
                                  UMI_COMMAND_REQUIRES_TRUST |
                                  UMI_COMMAND_AUDITED,
                              build_run_all_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_BUILD_INVALIDATE,
                              "Invalidate Build Node", "Build",
                              "Mark a node pending after its inputs change.",
                              "studio.build.execute",
                              UMI_COMMAND_MUTATES_STATE |
                                  UMI_COMMAND_REQUIRES_TRUST |
                                  UMI_COMMAND_AUDITED,
                              build_invalidate_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_BUILD_REFRESH,
                              "Refresh Build Workspace", "Build",
                              "Refresh graph readiness, selection and totals.",
                              "studio.build.read", UMI_COMMAND_NONE,
                              build_refresh_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_BUILD_CLEAR_HISTORY,
                              "Clear Build History", "Build",
                              "Clear retained build results and output evidence.",
                              "studio.build.execute",
                              UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED,
                              build_clear_history_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TESTS_DISCOVER,
                              "Discover Tests", "Testing",
                              "Discover CTest tests from a build directory.",
                              "studio.tests.read", UMI_COMMAND_NONE,
                              tests_discover_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TESTS_FILTER,
                              "Filter Tests", "Testing",
                              "Filter tests by text or by all, passed, failed, skipped or not-run state.",
                              "studio.tests.read", UMI_COMMAND_NONE,
                              tests_filter_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TESTS_SELECT,
                              "Select Test", "Testing",
                              "Select a visible test by its stable identifier.",
                              "studio.tests.read", UMI_COMMAND_NONE,
                              tests_select_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TESTS_RUN_ALL,
                              "Run All Tests", "Testing",
                              "Execute every test visible in the Test Explorer.",
                              "studio.tests.execute", UMI_COMMAND_AUDITED,
                              tests_run_all_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TESTS_RUN_SELECTED,
                              "Run Selected Test", "Testing",
                              "Execute the selected Test Explorer item.",
                              "studio.tests.execute", UMI_COMMAND_AUDITED,
                              tests_run_selected_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TESTS_DEBUG_SELECTED,
                              "Debug Selected Test", "Testing",
                              "Execute the selected test with debugger run-mode context.",
                              "studio.tests.execute", UMI_COMMAND_AUDITED,
                              tests_debug_selected_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TESTS_RUN_COVERAGE,
                              "Run Tests with Coverage", "Testing",
                              "Execute visible tests with coverage run-mode context.",
                              "studio.tests.execute", UMI_COMMAND_AUDITED,
                              tests_run_coverage_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TESTS_RERUN_FAILED,
                              "Rerun Failed Tests", "Testing",
                              "Execute tests whose latest retained result failed.",
                              "studio.tests.execute", UMI_COMMAND_AUDITED,
                              tests_rerun_failed_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TESTS_REPEAT_SELECTED,
                              "Repeat Selected Test", "Testing",
                              "Repeat the selected test to diagnose intermittent failures.",
                              "studio.tests.execute", UMI_COMMAND_AUDITED,
                              tests_repeat_selected_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TESTS_STOP,
                              "Stop Test Run", "Testing",
                              "Request cooperative cancellation of the active test operation.",
                              "studio.tests.execute", UMI_COMMAND_AUDITED,
                              tests_stop_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TESTS_CLEAR_RESULTS,
                              "Clear Test Results", "Testing",
                              "Clear retained test-result evidence.",
                              "studio.tests.execute", UMI_COMMAND_MUTATES_STATE,
                              tests_clear_results_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TESTS_CLEAR_OUTPUT,
                              "Clear Test Output", "Testing",
                              "Clear retained test output streams.",
                              "studio.tests.execute", UMI_COMMAND_MUTATES_STATE,
                              tests_clear_output_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TESTS_CLEAR_COVERAGE,
                              "Clear Test Coverage", "Testing",
                              "Clear retained line and branch coverage summaries.",
                              "studio.tests.execute", UMI_COMMAND_MUTATES_STATE,
                              tests_clear_coverage_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TERMINAL_EXECUTE,
                              "Execute in Terminal", "Terminal",
                              "Execute a command in the prepared Studio terminal.",
                              "process.execute",
                              UMI_COMMAND_MUTATES_STATE |
                                  UMI_COMMAND_REQUIRES_TRUST |
                                  UMI_COMMAND_AUDITED,
                              terminal_execute_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TERMINAL_CLEAR,
                              "Clear Terminal", "Terminal",
                              "Clear the retained transcript for the primary terminal.",
                              "process.read",
                              UMI_COMMAND_MUTATES_STATE,
                              terminal_clear_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TERMINAL_NEW,
                              "New Terminal", "Terminal",
                              "Open and activate a terminal using a named profile.",
                              "process.execute",
                              UMI_COMMAND_MUTATES_STATE |
                                  UMI_COMMAND_REQUIRES_TRUST |
                                  UMI_COMMAND_AUDITED,
                              terminal_new_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TERMINAL_CLOSE,
                              "Close Terminal", "Terminal",
                              "Close the active terminal while retaining one session.",
                              "process.execute",
                              UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED,
                              terminal_close_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TERMINAL_NEXT,
                              "Next Terminal", "Terminal",
                              "Activate the next terminal tab.",
                              "process.read", UMI_COMMAND_NONE,
                              terminal_next_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TERMINAL_PREVIOUS,
                              "Previous Terminal", "Terminal",
                              "Activate the previous terminal tab.",
                              "process.read", UMI_COMMAND_NONE,
                              terminal_previous_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TERMINAL_SPLIT_HORIZONTAL,
                              "Split Terminal Horizontally", "Terminal",
                              "Create a horizontal split from the active terminal.",
                              "process.execute",
                              UMI_COMMAND_MUTATES_STATE |
                                  UMI_COMMAND_REQUIRES_TRUST |
                                  UMI_COMMAND_AUDITED,
                              terminal_split_horizontal_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TERMINAL_SPLIT_VERTICAL,
                              "Split Terminal Vertically", "Terminal",
                              "Create a vertical split from the active terminal.",
                              "process.execute",
                              UMI_COMMAND_MUTATES_STATE |
                                  UMI_COMMAND_REQUIRES_TRUST |
                                  UMI_COMMAND_AUDITED,
                              terminal_split_vertical_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TERMINAL_HISTORY_CLEAR,
                              "Clear Terminal History", "Terminal",
                              "Clear retained command history without closing sessions.",
                              "process.execute",
                              UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED,
                              terminal_history_clear_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TERMINAL_SEARCH,
                              "Search Terminal", "Terminal",
                              "Search the active retained terminal transcript.",
                              "process.read", UMI_COMMAND_NONE,
                              terminal_search_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_PROCESS_REPORT,
                              "Report Processes", "Processes",
                              "Report supervised process state and totals.",
                              "process.read", UMI_COMMAND_NONE,
                              process_report_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_PROCESS_CANCEL,
                              "Cancel Process", "Processes",
                              "Request cancellation of a supervised process ID.",
                              "process.execute",
                              UMI_COMMAND_MUTATES_STATE |
                                  UMI_COMMAND_REQUIRES_TRUST |
                                  UMI_COMMAND_AUDITED,
                              process_cancel_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_DIAGNOSTICS_CLEAR,
                              "Clear Problems", "Diagnostics",
                              "Clear retained Problems records.",
                              "studio.diagnostics.write",
                              UMI_COMMAND_MUTATES_STATE,
                              diagnostics_clear_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_OUTPUT_CLEAR,
                              "Clear Output", "Diagnostics",
                              "Clear retained build, terminal and runtime output.",
                              "studio.diagnostics.write",
                              UMI_COMMAND_MUTATES_STATE,
                              output_clear_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_LANGUAGE_INITIALIZE,
                              "Initialise Language Server", "Language",
                              "Send the Language Server Protocol initialise request.",
                              "studio.language.use", UMI_COMMAND_NONE,
                              language_initialize_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_LANGUAGE_WORKSPACE_SYMBOLS,
                              "Workspace Symbols", "Language",
                              "Search symbols through the active language server.",
                              "studio.language.use", UMI_COMMAND_NONE,
                              language_workspace_symbols_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_DEBUG_INITIALIZE,
                              "Initialise Debug Adapter", "Debug",
                              "Send the Debug Adapter Protocol initialise request.",
                              "studio.debug.use", UMI_COMMAND_NONE,
                              debug_initialize_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_DEBUG_START,
                              "Start Debugging", "Debug",
                              "Initialise the adapter and launch the active program.",
                              "studio.debug.use", UMI_COMMAND_AUDITED,
                              debug_start_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_DEBUG_CONTINUE,
                              "Continue", "Debug", "Continue the selected thread.",
                              "studio.debug.use", UMI_COMMAND_NONE,
                              debug_continue_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_DEBUG_PAUSE,
                              "Pause", "Debug", "Pause the selected thread.",
                              "studio.debug.use", UMI_COMMAND_NONE,
                              debug_pause_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_DEBUG_NEXT,
                              "Step Over", "Debug", "Step over on the selected thread.",
                              "studio.debug.use", UMI_COMMAND_NONE,
                              debug_next_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_DEBUG_STEP_IN,
                              "Step Into", "Debug", "Step into on the selected thread.",
                              "studio.debug.use", UMI_COMMAND_NONE,
                              debug_step_in_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_DEBUG_STEP_OUT,
                              "Step Out", "Debug", "Step out on the selected thread.",
                              "studio.debug.use", UMI_COMMAND_NONE,
                              debug_step_out_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_DEBUG_STOP,
                              "Stop Debugging", "Debug", "Terminate the debuggee.",
                              "studio.debug.use", UMI_COMMAND_AUDITED,
                              debug_stop_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_DEBUG_ADD_BREAKPOINT,
                              "Add Breakpoint", "Debug",
                              "Add a source breakpoint using path:line.",
                              "studio.debug.use", UMI_COMMAND_MUTATES_STATE,
                              debug_add_breakpoint_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_DEBUG_SET_BREAKPOINT_ENABLED,
                              "Enable or Disable Breakpoint", "Debug",
                              "Set breakpoint state using id=1 or id=0.",
                              "studio.debug.use", UMI_COMMAND_MUTATES_STATE,
                              debug_set_breakpoint_enabled_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_DEBUG_REMOVE_BREAKPOINT,
                              "Remove Breakpoint", "Debug",
                              "Remove a breakpoint by its stable ID.",
                              "studio.debug.use", UMI_COMMAND_MUTATES_STATE,
                              debug_remove_breakpoint_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_DEBUG_ADD_WATCH,
                              "Add Watch Expression", "Debug",
                              "Add an expression to the Watch pane.",
                              "studio.debug.use", UMI_COMMAND_MUTATES_STATE,
                              debug_add_watch_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_DEBUG_REMOVE_WATCH,
                              "Remove Watch Expression", "Debug",
                              "Remove a watch expression by its stable ID.",
                              "studio.debug.use", UMI_COMMAND_MUTATES_STATE,
                              debug_remove_watch_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_DEBUG_SELECT_THREAD,
                              "Select Debug Thread", "Debug",
                              "Select a thread in the debugger workspace.",
                              "studio.debug.use", UMI_COMMAND_NONE,
                              debug_select_thread_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_DEBUG_SELECT_FRAME,
                              "Select Stack Frame", "Debug",
                              "Select a stack frame in the debugger workspace.",
                              "studio.debug.use", UMI_COMMAND_NONE,
                              debug_select_frame_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_DEBUG_SELECT_SCOPE,
                              "Select Variable Scope", "Debug",
                              "Select a variable scope for the Variables pane.",
                              "studio.debug.use", UMI_COMMAND_NONE,
                              debug_select_scope_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_DEBUG_CLEAR_CONSOLE,
                              "Clear Debug Console", "Debug",
                              "Clear retained Debug Console entries.",
                              "studio.debug.use", UMI_COMMAND_MUTATES_STATE,
                              debug_clear_console_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_VCS_REFRESH,
                              "Refresh Source Control", "Source Control",
                              "Refresh Git branch, change and history state.",
                              "vcs.read", UMI_COMMAND_NONE,
                              vcs_refresh_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_STAGE,
                              "Stage Path", "Source Control", "Stage a repository-relative path.",
                              "vcs.write", UMI_COMMAND_MUTATES_STATE, vcs_stage_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_UNSTAGE,
                              "Unstage Path", "Source Control", "Remove a repository-relative path from the index.",
                              "vcs.write", UMI_COMMAND_MUTATES_STATE, vcs_unstage_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_STAGE_ALL,
                              "Stage All", "Source Control", "Stage all working-tree changes.",
                              "vcs.write", UMI_COMMAND_MUTATES_STATE, vcs_stage_all_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_UNSTAGE_ALL,
                              "Unstage All", "Source Control", "Remove all changes from the index.",
                              "vcs.write", UMI_COMMAND_MUTATES_STATE, vcs_unstage_all_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_DISCARD,
                              "Discard Path", "Source Control", "Discard a working-tree path after confirmation.",
                              "vcs.write", UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED, vcs_discard_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_COMMIT,
                              "Commit", "Source Control", "Create a commit using the supplied message.",
                              "vcs.write", UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED, vcs_commit_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_FETCH,
                              "Fetch", "Source Control", "Fetch and prune all configured remotes.",
                              "vcs.network", UMI_COMMAND_AUDITED, vcs_fetch_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_PULL,
                              "Pull", "Source Control", "Fast-forward the active branch.",
                              "vcs.network", UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED, vcs_pull_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_PUSH,
                              "Push", "Source Control", "Push the active branch.",
                              "vcs.network", UMI_COMMAND_AUDITED, vcs_push_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_BRANCH_CREATE,
                              "Create Branch", "Source Control", "Create and check out a validated branch name.",
                              "vcs.write", UMI_COMMAND_MUTATES_STATE, vcs_branch_create_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_BRANCH_CHECKOUT,
                              "Checkout Branch", "Source Control", "Switch to a validated branch name.",
                              "vcs.write", UMI_COMMAND_MUTATES_STATE, vcs_branch_checkout_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_BRANCH_DELETE,
                              "Delete Branch", "Source Control", "Safely delete a merged branch.",
                              "vcs.write", UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED, vcs_branch_delete_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_DIFF,
                              "Open Diff", "Source Control", "Load a working-tree path diff; prefix with --staged for index diff.",
                              "vcs.read", UMI_COMMAND_NONE, vcs_diff_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_FILTER,
                              "Filter Source Control Changes", "Source Control",
                              "Show all, staged, unstaged or conflicting changes.",
                              "vcs.read", UMI_COMMAND_NONE, vcs_filter_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_SELECT_CHANGE,
                              "Select Source Control Change", "Source Control",
                              "Select a repository-relative path in the workspace.",
                              "vcs.read", UMI_COMMAND_NONE, vcs_select_change_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_SELECT_COMMIT,
                              "Select Repository Commit", "Source Control",
                              "Select a commit by its stable identifier.",
                              "vcs.read", UMI_COMMAND_NONE, vcs_select_commit_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_SELECT_BRANCH,
                              "Select Repository Branch", "Source Control",
                              "Select a branch without changing the working tree.",
                              "vcs.read", UMI_COMMAND_NONE, vcs_select_branch_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_SELECT_REMOTE,
                              "Select Repository Remote", "Source Control",
                              "Select a configured remote for inspection.",
                              "vcs.read", UMI_COMMAND_NONE, vcs_select_remote_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_SET_COMMIT_MESSAGE,
                              "Set Commit Message", "Source Control",
                              "Set the professional commit-composition message.",
                              "vcs.write", UMI_COMMAND_MUTATES_STATE, vcs_set_commit_message_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_STAGE_SELECTED,
                              "Stage Selected Change", "Source Control",
                              "Stage the selected repository path.",
                              "vcs.write", UMI_COMMAND_MUTATES_STATE, vcs_stage_selected_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_UNSTAGE_SELECTED,
                              "Unstage Selected Change", "Source Control",
                              "Remove the selected repository path from the index.",
                              "vcs.write", UMI_COMMAND_MUTATES_STATE, vcs_unstage_selected_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_DISCARD_SELECTED,
                              "Discard Selected Change", "Source Control",
                              "Discard the selected unstaged path after confirmation.",
                              "vcs.write", UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED, vcs_discard_selected_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_COMMIT_COMPOSED,
                              "Commit Composed Changes", "Source Control",
                              "Commit staged changes using the composed message.",
                              "vcs.write", UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED, vcs_commit_composed_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_DIFF_SELECTED,
                              "Open Selected Working Tree Diff", "Source Control",
                              "Load the selected working-tree path diff.",
                              "vcs.read", UMI_COMMAND_NONE, vcs_diff_selected_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services, UMI_STUDIO_COMMAND_VCS_DIFF_SELECTED_STAGED,
                              "Open Selected Staged Diff", "Source Control",
                              "Load the selected index path diff.",
                              "vcs.read", UMI_COMMAND_NONE, vcs_diff_selected_staged_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TRADING_REFRESH,
                              "Refresh Trading Workspace", "Trading",
                              "Reconcile trading selections, derived metrics and capabilities.",
                              "studio.trading.read", UMI_COMMAND_NONE,
                              trading_refresh_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TRADING_FILTER_INSTRUMENTS,
                              "Filter Trading Instruments", "Trading",
                              "Filter the watchlist by symbol, venue, currency or identifier.",
                              "studio.trading.read", UMI_COMMAND_NONE,
                              trading_filter_instruments_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TRADING_SELECT_INSTRUMENT,
                              "Select Trading Instrument", "Trading",
                              "Select the instrument shared by depth, charts and order entry.",
                              "studio.trading.read", UMI_COMMAND_NONE,
                              trading_select_instrument_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TRADING_SET_ENVIRONMENT,
                              "Set Trading Environment", "Trading",
                              "Select simulation, paper or live while retaining readiness gates.",
                              "studio.trading.use", UMI_COMMAND_MUTATES_STATE,
                              trading_set_environment_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TRADING_SET_SIDE,
                              "Set Draft Order Side", "Trading",
                              "Set the draft order to buy or sell.",
                              "studio.trading.use", UMI_COMMAND_MUTATES_STATE,
                              trading_set_side_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TRADING_SET_TYPE,
                              "Set Draft Order Type", "Trading",
                              "Set type and time in force using type[:tif].",
                              "studio.trading.use", UMI_COMMAND_MUTATES_STATE,
                              trading_set_type_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TRADING_SET_QUANTITY,
                              "Set Draft Order Quantity", "Trading",
                              "Set a positive draft order quantity.",
                              "studio.trading.use", UMI_COMMAND_MUTATES_STATE,
                              trading_set_quantity_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TRADING_SET_PRICES,
                              "Set Draft Order Prices", "Trading",
                              "Set limit[:stop] prices for the draft order.",
                              "studio.trading.use", UMI_COMMAND_MUTATES_STATE,
                              trading_set_prices_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TRADING_PREVIEW_ORDER,
                              "Preview Order Risk", "Trading",
                              "Evaluate pre-trade limits without submitting an order.",
                              "studio.trading.read", UMI_COMMAND_NONE,
                              trading_preview_order_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TRADING_SUBMIT_ORDER,
                              "Submit Draft Order", "Trading",
                              "Submit only when environment, health, risk and kill-switch gates permit.",
                              "studio.trading.execute",
                              UMI_COMMAND_MUTATES_STATE |
                                  UMI_COMMAND_REQUIRES_TRUST |
                                  UMI_COMMAND_AUDITED,
                              trading_submit_order_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TRADING_FILTER_ORDERS,
                              "Filter Trading Orders", "Trading",
                              "Show all, open, filled, cancelled or rejected orders.",
                              "studio.trading.read", UMI_COMMAND_NONE,
                              trading_filter_orders_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TRADING_SELECT_ORDER,
                              "Select Trading Order", "Trading",
                              "Select an order by its stable client order identifier.",
                              "studio.trading.read", UMI_COMMAND_NONE,
                              trading_select_order_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TRADING_CANCEL_ORDER,
                              "Cancel Selected Order", "Trading",
                              "Cancel the selected non-terminal order.",
                              "studio.trading.execute",
                              UMI_COMMAND_MUTATES_STATE |
                                  UMI_COMMAND_REQUIRES_TRUST |
                                  UMI_COMMAND_AUDITED,
                              trading_cancel_order_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TRADING_ENGAGE_KILL_SWITCH,
                              "Engage Trading Kill Switch", "Trading",
                              "Reject new orders immediately and retain the operator reason.",
                              "studio.trading.execute",
                              UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED,
                              trading_engage_kill_switch_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_TRADING_RESET_KILL_SWITCH,
                              "Reset Trading Kill Switch", "Trading",
                              "Reset the emergency stop after its cause has been reviewed.",
                              "studio.trading.execute",
                              UMI_COMMAND_MUTATES_STATE |
                                  UMI_COMMAND_REQUIRES_TRUST |
                                  UMI_COMMAND_AUDITED,
                              trading_reset_kill_switch_handler);
    if (status != UMI_STATUS_OK) return status;
    status = register_command(registry, services,
                              UMI_STUDIO_COMMAND_DEVELOPER_REPORT,
                              "Developer Platform Report", "Development",
                              "Report build, tests, terminal, language, debug and Git state.",
                              "studio.developer.read", UMI_COMMAND_NONE,
                              developer_report_handler);
    return status == UMI_STATUS_OK
        ? umi_studio_knowledge_commands_register(registry, services) : status;
}
