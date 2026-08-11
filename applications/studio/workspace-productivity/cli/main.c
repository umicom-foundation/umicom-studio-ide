/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/cli/main.c
 *
 * PURPOSE:
 *   Provide a headless demonstration/inspection command for workspace productivity models.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is part of Studio workspace/editor productivity. The implementation
 * uses bounded C23 data structures and explicit ownership so the behaviour is
 * easy to inspect, test and later connect to the main Studio workbench.
 */

#include <stdio.h>
#include <string.h>

#include "umicom/studio/workspace_productivity.h"

static void populate(UmiStudioSessionState *session,
                     UmiStudioRecentWorkspaceList *recent)
{
    umi_studio_session_state_init(session);
    umi_studio_recent_workspaces_init(recent);

    (void)umi_studio_session_state_set_workspace(
        session,
        "C:/Dev/umicom/umicom-studio"
    );

    (void)umi_studio_document_set_open(
        &session->documents,
        "applications/studio/src/gtk/main.c",
        "C",
        false
    );
    (void)umi_studio_document_set_open(
        &session->documents,
        "applications/studio/CMakeLists.txt",
        "CMake",
        true
    );
    (void)umi_studio_document_set_pin(
        &session->documents,
        "applications/studio/src/gtk/main.c",
        true
    );

    (void)umi_studio_recent_workspaces_touch(
        recent,
        "C:/Dev/umicom/umicom-studio",
        "Umicom Studio"
    );
}

static int command_summary(void)
{
    UmiStudioSessionState session;
    UmiStudioRecentWorkspaceList recent;
    UmiStudioWorkspaceSummary summary;

    populate(&session, &recent);

    if (umi_studio_workspace_summary_build(&session.documents,
                                           &recent,
                                           &summary) != UMI_STATUS_OK) {
        return 1;
    }

    (void)printf("Workspace        : %s\n", session.workspace_path);
    (void)printf("Open documents   : %zu\n", summary.open_documents);
    (void)printf("Dirty documents  : %zu\n", summary.dirty_documents);
    (void)printf("Pinned documents : %zu\n", summary.pinned_documents);
    (void)printf("Preview documents: %zu\n", summary.preview_documents);
    (void)printf("Recent workspaces: %zu\n", summary.recent_workspaces);
    return 0;
}

static int command_quick_open(const char *query)
{
    UmiStudioQuickOpenCandidate candidates[] = {
        {"main.c", "applications/studio/src/gtk/main.c",
         UMI_STUDIO_QUICK_OPEN_FILE, 0},
        {"CMakeLists.txt", "applications/studio/CMakeLists.txt",
         UMI_STUDIO_QUICK_OPEN_FILE, 0},
        {"runtime_manager.c",
         "applications/studio/runtime-manager/src/runtime_manager.c",
         UMI_STUDIO_QUICK_OPEN_FILE, 0},
        {"window.c", "applications/studio/src/gui/app/window.c",
         UMI_STUDIO_QUICK_OPEN_FILE, 0}
    };
    UmiStudioQuickOpenResults results;
    size_t index;

    if (umi_studio_quick_open_search(
            query,
            candidates,
            sizeof(candidates) / sizeof(candidates[0]),
            &results) != UMI_STATUS_OK) {
        return 1;
    }

    for (index = 0U; index < results.count; ++index) {
        (void)printf("%4d  %-22s %s\n",
                     results.items[index].score,
                     results.items[index].label,
                     results.items[index].detail);
    }

    return 0;
}

static int command_session(void)
{
    UmiStudioSessionState session;
    UmiStudioSessionState restored;
    UmiStudioRecentWorkspaceList recent;
    char encoded[UMI_STUDIO_SESSION_TEXT_CAPACITY];

    populate(&session, &recent);
    (void)recent;

    if (umi_studio_session_state_encode(&session,
                                        encoded,
                                        sizeof(encoded)) != UMI_STATUS_OK) {
        return 1;
    }

    (void)puts(encoded);

    if (umi_studio_session_state_decode(encoded, &restored) != UMI_STATUS_OK) {
        return 1;
    }

    (void)printf("Restored documents: %zu\n", restored.documents.count);
    return 0;
}

static int command_find(const char *query)
{
    const char *sample =
        "Umicom Studio makes C development productive. "
        "Studio keeps source editing explicit and fast.";
    UmiStudioFindReplace operation;
    size_t count = 0U;

    umi_studio_find_replace_init(&operation);
    if (umi_studio_find_replace_set(&operation, query, "") != UMI_STATUS_OK) {
        return 1;
    }

    if (umi_studio_find_count(sample, &operation, &count) != UMI_STATUS_OK) {
        return 1;
    }

    (void)printf("Matches: %zu\n", count);
    return 0;
}

static void usage(void)
{
    (void)puts("Usage:");
    (void)puts("  umicom-studio-workspace-tools summary");
    (void)puts("  umicom-studio-workspace-tools quick-open <query>");
    (void)puts("  umicom-studio-workspace-tools session");
    (void)puts("  umicom-studio-workspace-tools find <query>");
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        usage();
        return 0;
    }

    if (strcmp(argv[1], "summary") == 0) {
        return command_summary();
    }
    if (strcmp(argv[1], "quick-open") == 0 && argc >= 3) {
        return command_quick_open(argv[2]);
    }
    if (strcmp(argv[1], "session") == 0) {
        return command_session();
    }
    if (strcmp(argv[1], "find") == 0 && argc >= 3) {
        return command_find(argv[2]);
    }

    usage();
    return 2;
}
