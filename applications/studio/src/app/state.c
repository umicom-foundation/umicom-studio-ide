/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/state.c
 *
 * PURPOSE:
 *   Capture and format a consistent high-level Studio operational report from
 *   the shared Framework registries and product service container.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/state.h"
#include "umicom/studio/tasks.h"

#include "umicom/studio/data.h"
#include "umicom/studio/messages.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_studio_state_capture(UmiStudioBootstrap *bootstrap,
                                   UmiStudioStateReport *out_report)
{
    UmiStudioServices *services;
    UmiServiceRegistry *service_registry;
    UmiCommandRegistry *command_registry;
    UmiHealthRegistry *health_registry;

    if (bootstrap == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    services = umi_studio_bootstrap_services(bootstrap);
    service_registry = umi_studio_bootstrap_service_registry(bootstrap);
    command_registry = umi_studio_bootstrap_command_registry(bootstrap);
    health_registry = umi_studio_bootstrap_health_registry(bootstrap);
    if (services == NULL || service_registry == NULL ||
        command_registry == NULL || health_registry == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->module_count = umi_studio_bootstrap_module_count(bootstrap);
    out_report->service_count =
        umi_service_registry_count(service_registry);
    out_report->command_count =
        umi_command_registry_count(command_registry);
    out_report->health_count =
        umi_health_registry_count(health_registry);
    out_report->overall_health =
        umi_health_registry_overall(health_registry);
    out_report->tasks = umi_studio_tasks_stats(services);
    out_report->document_count = umi_document_store_count(
        umi_studio_services_documents(services)
    );
    out_report->session_entry_count = umi_session_store_count(
        umi_studio_services_session(services)
    );
    (void)umi_workspace_graph_snapshot(
        umi_studio_services_workspace(services),
        &out_report->workspace);
    out_report->files = umi_file_index_stats(
        umi_studio_services_file_index(services));
    out_report->watcher = umi_watcher_stats(
        umi_studio_services_watcher(services));
    out_report->processes = umi_process_supervisor_stats(
        umi_studio_services_process_supervisor(services));
    (void)umi_studio_operations_report(
        umi_studio_services_operations(services),
        &out_report->operations);
    {
        UmiStudioDataReport data;
        UmiStudioMessageReport messages;
        if (umi_studio_data_report(services, &data) == UMI_STATUS_OK) {
            out_report->data_records = data.records;
        }
        if (umi_studio_messages_report(services, &messages) == UMI_STATUS_OK) {
            out_report->message_schemas = messages.schemas;
            out_report->message_topics = messages.topics;
            out_report->journal_messages = messages.journal_messages;
            out_report->outbox = messages.outbox;
            out_report->dead_letters = messages.dead_letters;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_state_format(const UmiStudioStateReport *report,
                                  char *out_text,
                                  size_t text_capacity)
{
    int written;

    if (report == NULL || out_text == NULL || text_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(
        out_text,
        text_capacity,
        "Modules: %zu\n"
        "Services: %zu\n"
        "Commands: %zu\n"
        "Health records: %zu\n"
        "Overall health: %s\n"
        "Task workers: %zu\n"
        "Tasks queued: %zu\n"
        "Tasks running: %zu\n"
        "Documents: %zu\n"
        "Session entries: %zu\n"
        "Workspace open: %s\n"
        "Workspace trusted: %s\n"
        "Workspace projects: %zu\n"
        "Indexed files: %zu\n"
        "Watcher running: %s\n"
        "Watcher events: %llu\n"
        "Process jobs: %zu\n"
        "Process jobs running: %zu\n"
        "Data records: %zu\n"
        "Message schemas: %zu\n"
        "Message topics: %zu\n"
        "Journal messages: %zu\n"
        "Outbox pending: %zu\n"
        "Dead letters: %zu\n"
        "Security identities: %zu\n"
        "Plug-ins: %zu\n"
        "Operational metrics: %zu\n"
        "Operational events: %zu\n"
        "Supervised components: %zu\n"
        "Operations ready: %s\n",
        report->module_count,
        report->service_count,
        report->command_count,
        report->health_count,
        umi_health_state_text(report->overall_health),
        report->tasks.worker_count,
        report->tasks.queued,
        report->tasks.running,
        report->document_count,
        report->session_entry_count,
        report->workspace.open ? "yes" : "no",
        report->workspace.trusted ? "yes" : "no",
        report->workspace.project_count,
        report->files.files,
        report->watcher.running ? "yes" : "no",
        (unsigned long long)report->watcher.events,
        report->processes.jobs,
        report->processes.running,
        report->data_records,
        report->message_schemas,
        report->message_topics,
        report->journal_messages,
        report->outbox.pending,
        report->dead_letters,
        report->operations.identities,
        report->operations.plugins,
        report->operations.metrics,
        report->operations.operational_events,
        report->operations.supervised_components,
        report->operations.ready ? "yes" : "no"
    );

    return written < 0 || (size_t)written >= text_capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}
