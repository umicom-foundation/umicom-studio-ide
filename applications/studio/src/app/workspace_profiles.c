/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/workspace_profiles.c
 *
 * PURPOSE:
 *   Register professional named workspace profiles with the live Framework
 *   workbench used by GTK4, headless tests and future frontend adapters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * These profiles change only visible workbench chrome. They do not delete,
 * move or rewrite project files. Users can switch profiles from the toolbar,
 * the Window menu or the shared command registry, and the active selection is
 * saved in the normal Studio session state.
 */

#include "umicom/studio/workspace_profiles.h"

#include <stdio.h>
#include <string.h>

#include "umicom/studio/contributions.h"

static int is_debug_workspace_pane(const char *pane_id)
{
    return strcmp(pane_id, UMI_STUDIO_PANE_RUN_DEBUG) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_DEBUG_CALL_STACK) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_DEBUG_VARIABLES) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_DEBUG_WATCHES) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_DEBUG_BREAKPOINTS) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_DEBUG_CONSOLE) == 0;
}

static int is_source_control_workspace_pane(const char *pane_id)
{
    return strcmp(pane_id, UMI_STUDIO_PANE_SOURCE_CONTROL) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_VCS_COMMIT) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_VCS_HISTORY) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_VCS_BRANCHES) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_VCS_REMOTES) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_VCS_CONFLICTS) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_VCS_DIFF) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_VCS_OPERATIONS) == 0;
}

static int is_testing_workspace_pane(const char *pane_id)
{
    return strcmp(pane_id, UMI_STUDIO_PANE_TESTING) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_TEST_RESULTS) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_TEST_FAILURES) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_TEST_OUTPUT) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_TEST_COVERAGE) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_TEST_RUNS) == 0;
}

static int is_build_workspace_pane(const char *pane_id)
{
    return strcmp(pane_id, UMI_STUDIO_PANE_BUILD_DASHBOARD) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_BUILD_GRAPH) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_BUILD_HISTORY) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_BUILD_OUTPUT) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_BUILD_ARTIFACTS) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_BUILD_TASKS) == 0;
}

static int is_trading_workspace_pane(const char *pane_id)
{
    return strcmp(pane_id, UMI_STUDIO_PANE_TRADING_DASHBOARD) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_TRADING_WATCHLIST) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_TRADING_DEPTH) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_TRADING_CHART) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_TRADING_ORDER_TICKET) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_TRADING_ORDERS) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_TRADING_EXECUTIONS) == 0 ||
           strcmp(pane_id, UMI_STUDIO_PANE_TRADING_PORTFOLIO_RISK) == 0;
}

static UmiStatus register_profile(UmiUiWorkbench *workbench,
                                  const char *profile_id,
                                  const char *label,
                                  const char *description,
                                  const char *icon_name,
                                  int sidebar_visible,
                                  int auxiliary_visible,
                                  int bottom_visible,
                                  int32_t sidebar_size,
                                  int32_t auxiliary_size,
                                  int32_t bottom_size,
                                  int32_t order)
{
    UmiUiWorkspaceProfileSnapshot profile;
    UmiUiWorkspaceProfileModel *model;
    size_t pane_index;
    (void)memset(&profile, 0, sizeof(profile));
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model = umi_ui_workbench_workspace_profiles(workbench);
    (void)snprintf(profile.profile_id, sizeof(profile.profile_id), "%s",
                   profile_id);
    (void)snprintf(profile.label, sizeof(profile.label), "%s", label);
    (void)snprintf(profile.description, sizeof(profile.description), "%s",
                   description);
    (void)snprintf(profile.icon_name, sizeof(profile.icon_name), "%s",
                   icon_name);
    profile.sidebar_visible = sidebar_visible;
    profile.auxiliary_sidebar_visible = auxiliary_visible;
    profile.bottom_panel_visible = bottom_visible;
    profile.sidebar_size = sidebar_size;
    profile.auxiliary_sidebar_size = auxiliary_size;
    profile.bottom_panel_size = bottom_size;
    profile.order = order;
    profile.built_in = 1;
    profile.locked = 1;

    /* Built-in layouts remember Studio's original tool-tab positions. This
     * makes Reset useful after a drag operation while still preserving every
     * pane and source file. Optional panes that are added later remain
     * untouched because Framework ignores unknown IDs during activation. */
    for (pane_index = 0U;
         pane_index < umi_ui_pane_model_count(
             umi_ui_workbench_panes(workbench));
         ++pane_index) {
        UmiUiPaneSnapshot pane;
        UmiUiWorkspacePanePlacement *saved;
        UmiStatus status = umi_ui_pane_model_at(
            umi_ui_workbench_panes(workbench), pane_index, &pane);
        if (status != UMI_STATUS_OK) return status;
        int debug_pane = strcmp(profile_id,
                                UMI_STUDIO_WORKSPACE_PROFILE_DEBUG) == 0 &&
                         is_debug_workspace_pane(pane.pane_id);
        int source_control_pane =
            strcmp(profile_id,
                   UMI_STUDIO_WORKSPACE_PROFILE_SOURCE_CONTROL) == 0 &&
            is_source_control_workspace_pane(pane.pane_id);
        int testing_pane =
            strcmp(profile_id, UMI_STUDIO_WORKSPACE_PROFILE_TESTING) == 0 &&
            is_testing_workspace_pane(pane.pane_id);
        int build_pane =
            strcmp(profile_id, UMI_STUDIO_WORKSPACE_PROFILE_BUILD) == 0 &&
            is_build_workspace_pane(pane.pane_id);
        int trading_pane =
            strcmp(profile_id, UMI_STUDIO_WORKSPACE_PROFILE_TRADING) == 0 &&
            is_trading_workspace_pane(pane.pane_id);
        int profile_pane = debug_pane || source_control_pane || testing_pane ||
                           build_pane || trading_pane;
        if ((!pane.visible && !profile_pane) ||
            pane.placement == UMI_UI_PLACEMENT_CENTRE ||
            pane.placement == UMI_UI_PLACEMENT_FLOATING) {
            continue;
        }
        if (profile.pane_count >= UMI_UI_WORKSPACE_PROFILE_MAX_PANES) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        saved = &profile.panes[profile.pane_count++];
        (void)snprintf(saved->pane_id, sizeof(saved->pane_id), "%s",
                       pane.pane_id);
        saved->placement = pane.placement;
        saved->order = pane.order;
        saved->visible = profile_pane ? 1 : pane.visible;
    }
    return umi_ui_workspace_profile_model_upsert(model, &profile);
}

UmiStatus umi_studio_workspace_profiles_register(UmiUiWorkbench *workbench)
{
    UmiStatus status;

    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = register_profile(
        workbench, UMI_STUDIO_WORKSPACE_PROFILE_DEVELOP, "Develop",
        "Editor-first coding with project tools and a compact bottom panel",
        "applications-development-symbolic", 1, 0, 1, 288, 360, 240, 10);
    if (status != UMI_STATUS_OK) return status;
    status = register_profile(
        workbench, UMI_STUDIO_WORKSPACE_PROFILE_FOCUS, "Focus",
        "Distraction-free editor with all tool regions temporarily hidden",
        "view-fullscreen-symbolic", 0, 0, 0, 288, 360, 240, 20);
    if (status != UMI_STATUS_OK) return status;
    status = register_profile(
        workbench, UMI_STUDIO_WORKSPACE_PROFILE_DEBUG, "Debug",
        "Source, variables, call stack and an expanded debugging console",
        "applications-engineering-symbolic", 1, 1, 1, 300, 380, 300, 30);
    if (status != UMI_STATUS_OK) return status;
    status = register_profile(
        workbench, UMI_STUDIO_WORKSPACE_PROFILE_SOURCE_CONTROL,
        "Source Control",
        "Changes, commit composition, history, branches, remotes, conflicts, diffs and operations",
        "org.gnome.Builder-vcs-symbolic", 1, 1, 1, 340, 420, 320, 40);
    if (status != UMI_STATUS_OK) return status;
    status = register_profile(
        workbench, UMI_STUDIO_WORKSPACE_PROFILE_TESTING, "Testing",
        "Test Explorer, results, failures, output, coverage and run history",
        "emblem-ok-symbolic", 1, 1, 1, 340, 420, 320, 45);
    if (status != UMI_STATUS_OK) return status;
    status = register_profile(
        workbench, UMI_STUDIO_WORKSPACE_PROFILE_BUILD, "Build",
        "Build dashboard, dependency graph, history, output, artifacts and tasks",
        "system-run-symbolic", 1, 1, 1, 360, 440, 340, 47);
    if (status != UMI_STATUS_OK) return status;
    status = register_profile(
        workbench, UMI_STUDIO_WORKSPACE_PROFILE_REVIEW, "Review",
        "Code review and comparison with navigation and auxiliary context",
        "document-properties-symbolic", 1, 1, 1, 260, 420, 260, 50);
    if (status != UMI_STATUS_OK) return status;
    status = register_profile(
        workbench, UMI_STUDIO_WORKSPACE_PROFILE_OPERATIONS, "Operations",
        "Monitoring, diagnostics, tasks and operational evidence",
        "utilities-system-monitor-symbolic", 1, 1, 1, 280, 420, 320, 60);
    if (status != UMI_STATUS_OK) return status;
    status = register_profile(
        workbench, UMI_STUDIO_WORKSPACE_PROFILE_TRADING, "Trading",
        "TWS-inspired market, chart, order, risk and activity workspace",
        "view-statistics-symbolic", 1, 1, 1, 260, 390, 300, 70);
    if (status != UMI_STATUS_OK) return status;

    return umi_ui_workbench_activate_workspace_profile(
        workbench, UMI_STUDIO_WORKSPACE_PROFILE_DEVELOP);
}
