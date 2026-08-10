/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/developer_platform.c
 *
 * PURPOSE:
 *   Compose the Framework-backed developer services used by Studio tools and frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/developer_platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiStudioDeveloperPlatform {
    UmiStudioBuildService *build;
    UmiStudioTestService *tests;
    UmiStudioTerminalService *terminal;
    UmiStudioLanguageService *language;
    UmiStudioDebuggerService *debugger;
    UmiStudioSourceControlService *source_control;
};

static UmiStatus root_to_uri(const char *root, char *out_uri, size_t capacity)
{
    size_t input_index;
    size_t output_index;
    const char *prefix;
    size_t prefix_length;

    if (root == NULL || root[0] == '\0' || out_uri == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    prefix = root[1] == ':' ? "file:///" : "file://";
    prefix_length = strlen(prefix);
    if (prefix_length + strlen(root) + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(out_uri, prefix, prefix_length);
    output_index = prefix_length;
    for (input_index = 0U; root[input_index] != '\0'; ++input_index) {
        out_uri[output_index++] = root[input_index] == '\\'
            ? '/' : root[input_index];
    }
    out_uri[output_index] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_developer_platform_create(
    const char *workspace_root,
    UmiClock *clock,
    UmiStudioDeveloperPlatform **out_platform)
{
    UmiStudioDeveloperPlatform *platform;
    UmiStatus status;
    char root_uri[UMI_PROTOCOL_URI_CAPACITY];
    if (workspace_root == NULL || clock == NULL || out_platform == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_platform = NULL;
    platform = (UmiStudioDeveloperPlatform *)calloc(1U, sizeof(*platform));
    if (platform == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = root_to_uri(workspace_root, root_uri, sizeof(root_uri));
    if (status == UMI_STATUS_OK) {
        status = umi_studio_build_service_create(workspace_root, clock,
                                                 &platform->build);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_test_service_create(&platform->tests);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_terminal_service_create(workspace_root, clock,
                                                    &platform->terminal);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_language_service_create(root_uri,
                                                    &platform->language);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_debugger_service_create(&platform->debugger);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_source_control_service_create(
            workspace_root, &platform->source_control);
    }
    if (status != UMI_STATUS_OK) {
        umi_studio_developer_platform_destroy(platform);
        return status;
    }
    *out_platform = platform;
    return UMI_STATUS_OK;
}

void umi_studio_developer_platform_destroy(
    UmiStudioDeveloperPlatform *platform)
{
    if (platform == NULL) return;
    umi_studio_source_control_service_destroy(platform->source_control);
    umi_studio_debugger_service_destroy(platform->debugger);
    umi_studio_language_service_destroy(platform->language);
    umi_studio_terminal_service_destroy(platform->terminal);
    umi_studio_test_service_destroy(platform->tests);
    umi_studio_build_service_destroy(platform->build);
    free(platform);
}

UmiStatus umi_studio_developer_platform_snapshot(
    const UmiStudioDeveloperPlatform *platform,
    UmiStudioDeveloperSnapshot *out_snapshot)
{
    UmiStatus status;
    if (platform == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    status = umi_studio_build_service_snapshot(platform->build,
                                               &out_snapshot->build);
    if (status == UMI_STATUS_OK) status = umi_studio_test_service_snapshot(platform->tests, &out_snapshot->tests);
    if (status == UMI_STATUS_OK) status = umi_studio_terminal_service_snapshot(platform->terminal, &out_snapshot->terminal);
    if (status == UMI_STATUS_OK) status = umi_studio_language_service_snapshot(platform->language, &out_snapshot->language);
    if (status == UMI_STATUS_OK) status = umi_studio_debugger_service_snapshot(platform->debugger, &out_snapshot->debugger);
    if (status == UMI_STATUS_OK) status = umi_studio_source_control_service_snapshot(platform->source_control, &out_snapshot->source_control);
    return status;
}

UmiStatus umi_studio_developer_platform_report(
    const UmiStudioDeveloperPlatform *platform,
    char *out_text,
    size_t capacity)
{
    UmiStudioDeveloperSnapshot snapshot;
    int written;
    UmiStatus status = umi_studio_developer_platform_snapshot(platform,
                                                              &snapshot);
    if (status != UMI_STATUS_OK || out_text == NULL || capacity == 0U) {
        return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(out_text, capacity,
        "Build profile: %s\nBuild history: %zu\n"
        "Test suites: %zu\nTests: %zu\n"
        "Terminal sessions: %zu\n"
        "Language documents: %zu\nLanguage messages: %zu\n"
        "Debugger breakpoints: %zu\nDebugger messages: %zu\n"
        "Source control: %s\nBranch: %s\nChanges: %zu\n",
        snapshot.build.profile_id,
        snapshot.build.history_count,
        snapshot.tests.suite_count,
        snapshot.tests.test_count,
        snapshot.terminal.session_count,
        snapshot.language.open_documents,
        snapshot.language.queued_messages,
        snapshot.debugger.breakpoint_count,
        snapshot.debugger.queued_messages,
        snapshot.source_control.available ? "available" : "unavailable",
        snapshot.source_control.branch,
        snapshot.source_control.changes);
    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

UmiStudioBuildService *umi_studio_developer_platform_build(
    UmiStudioDeveloperPlatform *platform)
{
    return platform != NULL ? platform->build : NULL;
}

UmiStudioTestService *umi_studio_developer_platform_tests(
    UmiStudioDeveloperPlatform *platform)
{
    return platform != NULL ? platform->tests : NULL;
}

UmiStudioTerminalService *umi_studio_developer_platform_terminal(
    UmiStudioDeveloperPlatform *platform)
{
    return platform != NULL ? platform->terminal : NULL;
}

UmiStudioLanguageService *umi_studio_developer_platform_language(
    UmiStudioDeveloperPlatform *platform)
{
    return platform != NULL ? platform->language : NULL;
}

UmiStudioDebuggerService *umi_studio_developer_platform_debugger(
    UmiStudioDeveloperPlatform *platform)
{
    return platform != NULL ? platform->debugger : NULL;
}

UmiStudioSourceControlService *umi_studio_developer_platform_source_control(
    UmiStudioDeveloperPlatform *platform)
{
    return platform != NULL ? platform->source_control : NULL;
}
