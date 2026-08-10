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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "umicom/studio/data.h"
#include "umicom/studio/documents.h"
#include "umicom/studio/messages.h"
#include "umicom/studio/observability.h"
#include "umicom/studio/plugins.h"
#include "umicom/studio/resilience.h"
#include "umicom/studio/security.h"
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
                              UMI_STUDIO_COMMAND_OBSERVABILITY_REPORT,
                              "Observability report",
                              "Operations",
                              "Inspect metrics, traces, audit, readiness and events.",
                              "studio.observability.read",
                              UMI_COMMAND_NONE,
                              observability_report_handler);
    if (status != UMI_STATUS_OK) return status;

    return register_command(registry,
                            services,
                            UMI_STUDIO_COMMAND_RESILIENCE_REPORT,
                            "Resilience report",
                            "Operations",
                            "Inspect supervised components, circuits and rate limits.",
                            "studio.resilience.read",
                            UMI_COMMAND_NONE,
                            resilience_report_handler);
}
