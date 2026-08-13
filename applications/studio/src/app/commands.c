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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/studio/build.h"
#include "umicom/studio/data.h"
#include "umicom/studio/debugger.h"
#include "umicom/studio/developer_platform.h"
#include "umicom/studio/diagnostics.h"
#include "umicom/studio/documents.h"
#include "umicom/studio/language.h"
#include "umicom/studio/messages.h"
#include "umicom/studio/observability.h"
#include "umicom/studio/plugins.h"
#include "umicom/studio/resilience.h"
#include "umicom/studio/security.h"
#include "umicom/studio/source_control.h"
#include "umicom/studio/terminal.h"
#include "umicom/studio/tests.h"
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

static UmiStatus build_phase_handler(UmiStudioServices *services,
                                     UmiBuildPhase phase,
                                     char *out_message,
                                     size_t message_capacity)
{
    UmiBuildResult result;
    UmiStatus status = umi_studio_build_service_run(
        umi_studio_services_build(services), phase, &result);
    UmiStatus diagnostic_status =
        umi_studio_diagnostics_ingest_build_result(services, &result);
    if (status == UMI_STATUS_OK && diagnostic_status != UMI_STATUS_OK) {
        status = diagnostic_status;
    }
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "%s: %s (exit %d, %zu diagnostic(s))",
                       umi_build_phase_text(phase),
                       umi_status_text(status),
                       result.exit_code,
                       result.diagnostics.count);
    }
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

static UmiStatus tests_discover_handler(void *user_data,
                                        const char *argument,
                                        char *out_message,
                                        size_t message_capacity)
{
    size_t discovered = 0U;
    const UmiBuildProfile *profile;
    UmiStatus status;
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    profile = umi_studio_build_service_profile(
        umi_studio_services_build(services));
    status = umi_studio_test_service_discover(
        umi_studio_services_tests(services),
        argument != NULL && argument[0] != '\0'
            ? argument : profile->build_directory,
        &discovered);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message,
                       message_capacity,
                       "Discovered %zu test(s): %s",
                       discovered,
                       umi_status_text(status));
    }
    return status;
}

static UmiStatus terminal_execute_handler(void *user_data,
                                          const char *argument,
                                          char *out_message,
                                          size_t message_capacity)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    char output_line[512];
    int exit_code = 0;
    UmiStatus status;
    if (argument == NULL || argument[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_studio_terminal_service_execute(
        umi_studio_services_terminal(services),
        argument,
        30000U,
        NULL,
        &exit_code);
    (void)snprintf(output_line, sizeof(output_line),
                   "Command '%s' exited with %d: %s",
                   argument, exit_code, umi_status_text(status));
    {
        UmiStatus output_status = umi_diagnostic_pipeline_ingest_line(
            umi_studio_services_diagnostic_pipeline(services),
            "terminal", "Terminal", "Umicom Terminal",
            status == UMI_STATUS_OK ? UMI_OUTPUT_STREAM_STANDARD : UMI_OUTPUT_STREAM_ERROR,
            output_line, 0U);
        if (status == UMI_STATUS_OK && output_status != UMI_STATUS_OK) status = output_status;
    }
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
    UmiStudioTerminalService *service = umi_studio_services_terminal(
        (UmiStudioServices *)user_data);
    UmiTerminalSession *session;
    UmiTerminalTranscript *transcript;
    (void)argument;
    if (service == NULL) return UMI_STATUS_INVALID_STATE;
    session = umi_studio_terminal_service_primary(service);
    transcript = umi_terminal_session_transcript(session);
    if (transcript == NULL) return UMI_STATUS_INVALID_STATE;
    umi_terminal_transcript_clear(transcript);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
                       "Terminal transcript cleared");
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
                              UMI_STUDIO_COMMAND_TESTS_DISCOVER,
                              "Discover Tests", "Testing",
                              "Discover CTest tests from a build directory.",
                              "studio.tests.read", UMI_COMMAND_NONE,
                              tests_discover_handler);
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
                              UMI_STUDIO_COMMAND_VCS_REFRESH,
                              "Refresh Source Control", "Source Control",
                              "Refresh Git branch, change and history state.",
                              "vcs.read", UMI_COMMAND_NONE,
                              vcs_refresh_handler);
    if (status != UMI_STATUS_OK) return status;
    return register_command(registry, services,
                            UMI_STUDIO_COMMAND_DEVELOPER_REPORT,
                            "Developer Platform Report", "Development",
                            "Report build, tests, terminal, language, debug and Git state.",
                            "studio.developer.read", UMI_COMMAND_NONE,
                            developer_report_handler);
}
