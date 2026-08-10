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
        "Session entries: %zu\n",
        report->module_count,
        report->service_count,
        report->command_count,
        report->health_count,
        umi_health_state_text(report->overall_health),
        report->tasks.worker_count,
        report->tasks.queued,
        report->tasks.running,
        report->document_count,
        report->session_entry_count
    );

    return written < 0 || (size_t)written >= text_capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}
