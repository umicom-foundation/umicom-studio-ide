/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/workbench_views.c
 *
 * PURPOSE:
 *   Provide live Studio view models for the unified Framework workbench so the
 *   Activity Bar opens useful product data rather than placeholder view-type
 *   labels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * These factories expose ordinary UmiUiViewModel properties. They never create
 * GTK widgets. The Framework GTK4 adapter can render them today, while a future
 * Qt, Wt or headless adapter can render the same models without rewriting the
 * Studio services underneath them.
 */

#include "umicom/studio/workbench_views.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "umicom/studio/build.h"
#include "umicom/studio/commands.h"
#include "umicom/studio/debugger.h"
#include "umicom/studio/debug_workspace_views.h"
#include "umicom/studio/designer.h"
#include "umicom/studio/extension_centre.h"
#include "umicom/studio/product_centre.h"
#include "umicom/studio/language.h"
#include "umicom/studio/knowledge_views.h"
#include "umicom/studio/source_control.h"
#include "umicom/studio/tests.h"
#include "umicom/studio/terminal.h"
#include "umicom/studio/trading.h"
#include "umicom/build_ui/build_ui.h"
#include "umicom/test_ui/test_ui.h"
#include "umicom/trading_ui/trading_ui.h"
#include "umicom/application_ui/application_ui.h"

#define VIEW_EXPLORER      "studio.project-explorer"
#define VIEW_SEARCH        "studio.search"
#define VIEW_SOURCE_CTRL   "studio.source-control"
#define VIEW_VCS_COMMIT    "studio.vcs-commit"
#define VIEW_VCS_HISTORY   "studio.vcs-history"
#define VIEW_VCS_BRANCHES  "studio.vcs-branches"
#define VIEW_VCS_REMOTES   "studio.vcs-remotes"
#define VIEW_VCS_CONFLICTS "studio.vcs-conflicts"
#define VIEW_VCS_DIFF      "studio.vcs-diff"
#define VIEW_VCS_OPERATIONS "studio.vcs-operations"
#define VIEW_RUN_DEBUG     "studio.run-debug"
#define VIEW_DEBUG_CALL_STACK "studio.debug-call-stack"
#define VIEW_DEBUG_VARIABLES "studio.debug-variables"
#define VIEW_DEBUG_WATCHES "studio.debug-watches"
#define VIEW_DEBUG_BREAKPOINTS "studio.debug-breakpoints"
#define VIEW_DEBUG_CONSOLE "studio.debug-console"
#define VIEW_TESTING       "studio.testing"
#define VIEW_TEST_RESULTS  "studio.test-results"
#define VIEW_TEST_FAILURES "studio.test-failures"
#define VIEW_TEST_OUTPUT   "studio.test-output"
#define VIEW_TEST_COVERAGE "studio.test-coverage"
#define VIEW_TEST_RUNS     "studio.test-runs"
#define VIEW_BUILD_DASHBOARD "studio.build-dashboard"
#define VIEW_BUILD_GRAPH "studio.build-graph"
#define VIEW_BUILD_HISTORY "studio.build-history"
#define VIEW_BUILD_OUTPUT "studio.build-output"
#define VIEW_BUILD_ARTIFACTS "studio.build-artifacts"
#define VIEW_BUILD_TASKS "studio.build-tasks"
#define VIEW_TRADING_DASHBOARD "studio.trading-dashboard"
#define VIEW_TRADING_WATCHLIST "studio.trading-watchlist"
#define VIEW_TRADING_DEPTH "studio.trading-depth"
#define VIEW_TRADING_CHART "studio.trading-chart"
#define VIEW_TRADING_ORDER_TICKET "studio.trading-order-ticket"
#define VIEW_TRADING_ORDERS "studio.trading-orders"
#define VIEW_TRADING_EXECUTIONS "studio.trading-executions"
#define VIEW_TRADING_PORTFOLIO_RISK "studio.trading-portfolio-risk"
#define VIEW_DESIGNER      "studio.designer"
#define VIEW_APPLICATIONS  "studio.application-hub"
#define VIEW_APPLICATION_COMPONENTS "studio.application-components"
#define VIEW_GTK4_COVERAGE "studio.gtk4-coverage"
#define VIEW_ARCHITECTURE "studio.architecture"
#define VIEW_FRAMEWORK     "studio.framework"
#define VIEW_AI            "studio.authorengine"
#define VIEW_AI_RUNTIMES   "studio.ai-runtimes"
#define VIEW_AI_CONTEXT    "studio.ai-context"
#define VIEW_AI_SESSIONS   "studio.ai-sessions"
#define VIEW_AI_PRIVACY    "studio.ai-privacy"
#define VIEW_AI_CODING     "studio.ai-coding"
#define VIEW_AI_CODING_CONTEXT "studio.ai-coding-context"
#define VIEW_AI_PATCH_REVIEW "studio.ai-patch-review"
#define VIEW_KNOWLEDGE "studio.knowledge"
#define VIEW_KNOWLEDGE_COLLECTIONS "studio.knowledge-collections"
#define VIEW_KNOWLEDGE_SOURCES "studio.knowledge-sources"
#define VIEW_KNOWLEDGE_SEARCH "studio.knowledge-search"
#define VIEW_KNOWLEDGE_SOURCE "studio.knowledge-source"
#define VIEW_OUTPUT        "studio.output"
#define VIEW_PROBLEMS      "studio.problems"
#define VIEW_TERMINAL      "studio.terminal"
#define VIEW_PROCESSES     "studio.processes"
#define VIEW_TASKS         "studio.tasks"
#define VIEW_HISTORY       "studio.terminal-history"
#define VIEW_EXTENSIONS    "studio.extensions-installed"
#define VIEW_EXT_CATALOGUE "studio.extensions-catalogue"
#define VIEW_EXT_PERMISSIONS "studio.extensions-permissions"
#define VIEW_EXT_AUDIT     "studio.extensions-audit"
#define VIEW_PRODUCT_MARKETPLACE "studio.product-marketplace"
#define VIEW_PRODUCTS_INSTALLED "studio.products-installed"
#define VIEW_PRODUCT_UPDATES "studio.product-updates"
#define VIEW_PRODUCT_TRANSACTIONS "studio.product-transactions"
#define VIEW_PRODUCT_EVIDENCE "studio.product-evidence"

static UmiStatus add_action_enabled(UmiUiViewModel *view,
                                    size_t index,
                                    const char *action_id,
                                    const char *label,
                                    const char *tooltip,
                                    int enabled)
{
    UmiUiCommandViewAction action = {0};
    (void)snprintf(action.action_id, sizeof(action.action_id), "%s", action_id);
    (void)snprintf(action.label, sizeof(action.label), "%s", label);
    (void)snprintf(action.tooltip, sizeof(action.tooltip), "%s", tooltip);
    action.enabled = enabled != 0;
    return umi_ui_command_view_set_action(view, index, &action);
}

static UmiStatus add_action(UmiUiViewModel *view,
                            size_t index,
                            const char *action_id,
                            const char *label,
                            const char *tooltip)
{
    return add_action_enabled(view, index, action_id, label, tooltip, 1);
}

typedef UmiStatus (*StudioViewCreateFn)(
    const char *view_id,
    void *user_data,
    UmiUiViewModel **out_view
);

static UmiStatus property_string(UmiUiViewModel *view,
                                 const char *key,
                                 const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(&value, text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value)
        : status;
}

static UmiStatus property_integer(UmiUiViewModel *view,
                                  const char *key,
                                  int64_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value)
        : status;
}

static UmiStatus property_boolean(UmiUiViewModel *view,
                                  const char *key,
                                  int enabled)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_boolean(&value, enabled);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value)
        : status;
}

static UmiStatus create_base_view(const char *view_id,
                                  const char *view_type,
                                  const char *title,
                                  const char *summary,
                                  UmiUiViewModel **out_view)
{
    UmiStatus status;

    status = umi_ui_view_model_create(
        view_id, view_type, UMI_UI_ROLE_PANE, out_view);
    if (status != UMI_STATUS_OK) return status;

    status = property_string(*out_view, "title", title);
    if (status == UMI_STATUS_OK) {
        status = property_string(*out_view, "summary", summary);
    }
    if (status != UMI_STATUS_OK) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

static UmiStatus create_explorer(const char *view_id,
                                 void *user_data,
                                 UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiFileIndexStats stats;
    UmiFileIndex *index;
    UmiStatus status = create_base_view(
        view_id,
        VIEW_EXPLORER,
        "Explorer",
        "Workspace files indexed by the reusable Framework file-index service.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    index = umi_studio_services_file_index(services);
    if (index == NULL) return property_boolean(*out_view, "available", 0);
    stats = umi_file_index_stats(index);

    status = property_boolean(*out_view, "available", 1);
    if (status == UMI_STATUS_OK) {
        status = property_string(*out_view, "workspace", stats.root);
    }
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "indexed-files",
                                  (int64_t)stats.files);
    }
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "index-revision",
                                  (int64_t)stats.revision);
    }
    return status;
}

static UmiStatus create_search(const char *view_id,
                               void *user_data,
                               UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiFileIndexStats stats;
    UmiFileIndex *index;
    UmiStatus status = create_base_view(
        view_id,
        VIEW_SEARCH,
        "Search",
        "Search the same indexed workspace used by Explorer and Quick Open.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    index = umi_studio_services_file_index(services);
    if (index == NULL) return property_boolean(*out_view, "available", 0);
    stats = umi_file_index_stats(index);

    status = property_boolean(*out_view, "available", 1);
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "searchable-files",
                              (int64_t)stats.files);
    }
    if (status == UMI_STATUS_OK) {
        status = property_string(*out_view, "root", stats.root);
    }
    return status;
}

static UmiStatus create_source_control(const char *view_id,
                                       void *user_data,
                                       UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiStudioSourceControlService *service;
    service = umi_studio_services_source_control(services);
    return service != NULL
        ? umi_vcs_ui_workspace_changes_view_create(
              view_id,
              umi_studio_source_control_service_coordinator(service),
              out_view)
        : UMI_STATUS_UNAVAILABLE;
}

static UmiVcsWorkspaceCoordinator *source_control_coordinator(void *user_data)
{
    UmiStudioSourceControlService *service = umi_studio_services_source_control(
        (UmiStudioServices *)user_data);
    return service != NULL
        ? umi_studio_source_control_service_coordinator(service) : NULL;
}

static UmiStatus create_vcs_commit(const char *view_id, void *user_data, UmiUiViewModel **out_view)
{ UmiVcsWorkspaceCoordinator *coordinator = source_control_coordinator(user_data); return coordinator != NULL ? umi_vcs_ui_workspace_commit_view_create(view_id, coordinator, out_view) : UMI_STATUS_UNAVAILABLE; }
static UmiStatus create_vcs_history(const char *view_id, void *user_data, UmiUiViewModel **out_view)
{ UmiVcsWorkspaceCoordinator *coordinator = source_control_coordinator(user_data); return coordinator != NULL ? umi_vcs_ui_workspace_history_view_create(view_id, coordinator, out_view) : UMI_STATUS_UNAVAILABLE; }
static UmiStatus create_vcs_branches(const char *view_id, void *user_data, UmiUiViewModel **out_view)
{ UmiVcsWorkspaceCoordinator *coordinator = source_control_coordinator(user_data); return coordinator != NULL ? umi_vcs_ui_workspace_branches_view_create(view_id, coordinator, out_view) : UMI_STATUS_UNAVAILABLE; }
static UmiStatus create_vcs_remotes(const char *view_id, void *user_data, UmiUiViewModel **out_view)
{ UmiVcsWorkspaceCoordinator *coordinator = source_control_coordinator(user_data); return coordinator != NULL ? umi_vcs_ui_workspace_remotes_view_create(view_id, coordinator, out_view) : UMI_STATUS_UNAVAILABLE; }
static UmiStatus create_vcs_conflicts(const char *view_id, void *user_data, UmiUiViewModel **out_view)
{ UmiVcsWorkspaceCoordinator *coordinator = source_control_coordinator(user_data); return coordinator != NULL ? umi_vcs_ui_workspace_conflicts_view_create(view_id, coordinator, out_view) : UMI_STATUS_UNAVAILABLE; }
static UmiStatus create_vcs_diff(const char *view_id, void *user_data, UmiUiViewModel **out_view)
{ UmiVcsWorkspaceCoordinator *coordinator = source_control_coordinator(user_data); return coordinator != NULL ? umi_vcs_ui_workspace_diff_view_create(view_id, coordinator, out_view) : UMI_STATUS_UNAVAILABLE; }
static UmiStatus create_vcs_operations(const char *view_id, void *user_data, UmiUiViewModel **out_view)
{ UmiVcsWorkspaceCoordinator *coordinator = source_control_coordinator(user_data); return coordinator != NULL ? umi_vcs_ui_workspace_operations_view_create(view_id, coordinator, out_view) : UMI_STATUS_UNAVAILABLE; }

static UmiStatus create_extensions(const char *view_id, void *user_data, UmiUiViewModel **out_view)
{ return umi_studio_extension_centre_installed_view((UmiStudioServices *)user_data, view_id, out_view); }
static UmiStatus create_extension_catalogue(const char *view_id, void *user_data, UmiUiViewModel **out_view)
{ return umi_studio_extension_centre_catalogue_view((UmiStudioServices *)user_data, view_id, out_view); }
static UmiStatus create_extension_permissions(const char *view_id, void *user_data, UmiUiViewModel **out_view)
{ return umi_studio_extension_centre_permissions_view((UmiStudioServices *)user_data, view_id, out_view); }
static UmiStatus create_extension_audit(const char *view_id, void *user_data, UmiUiViewModel **out_view)
{ return umi_studio_extension_centre_audit_view((UmiStudioServices *)user_data, view_id, out_view); }

/*
 * Product views deliberately delegate to Framework's toolkit-neutral
 * distribution UI.  Studio contributes stable view IDs and service lifetime;
 * it does not duplicate catalogue, policy or transaction presentation logic.
 */
static UmiStudioProductCentre *product_centre(void *user_data)
{
    return umi_studio_services_product_centre(
        (UmiStudioServices *)user_data);
}

static UmiStatus create_product_marketplace(
    const char *view_id,
    void *user_data,
    UmiUiViewModel **out_view)
{
    return umi_studio_product_centre_marketplace_view(
        product_centre(user_data), view_id, out_view);
}

static UmiStatus create_products_installed(
    const char *view_id,
    void *user_data,
    UmiUiViewModel **out_view)
{
    return umi_studio_product_centre_installed_view(
        product_centre(user_data), view_id, out_view);
}

static UmiStatus create_product_updates(
    const char *view_id,
    void *user_data,
    UmiUiViewModel **out_view)
{
    return umi_studio_product_centre_updates_view(
        product_centre(user_data), view_id, out_view);
}

static UmiStatus create_product_transactions(
    const char *view_id,
    void *user_data,
    UmiUiViewModel **out_view)
{
    return umi_studio_product_centre_transactions_view(
        product_centre(user_data), view_id, out_view);
}

static UmiStatus create_product_evidence(
    const char *view_id,
    void *user_data,
    UmiUiViewModel **out_view)
{
    return umi_studio_product_centre_evidence_view(
        product_centre(user_data), view_id, out_view);
}

static UmiStatus create_run_debug(const char *view_id,
                                  void *user_data,
                                  UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiStudioBuildSnapshot build_snapshot;
    UmiStudioDebuggerSnapshot debug_snapshot;
    UmiStudioBuildService *build;
    UmiStudioDebuggerService *debugger;
    UmiStatus status = create_base_view(
        view_id,
        VIEW_RUN_DEBUG,
        "Run and Debug",
        "Build-state and Debug Adapter Protocol state presented together.",
        out_view);

    if (status != UMI_STATUS_OK) return status;

    build = umi_studio_services_build(services);
    debugger = umi_studio_services_debugger(services);
    (void)memset(&build_snapshot, 0, sizeof(build_snapshot));
    (void)memset(&debug_snapshot, 0, sizeof(debug_snapshot));

    if (build != NULL &&
        umi_studio_build_service_snapshot(build, &build_snapshot) ==
            UMI_STATUS_OK) {
        status = property_string(*out_view, "build-directory",
                                 build_snapshot.build_directory);
        if (status == UMI_STATUS_OK) {
            status = property_integer(*out_view, "build-history",
                                      (int64_t)build_snapshot.history_count);
        }
        if (status == UMI_STATUS_OK) {
            status = property_integer(*out_view, "build-diagnostics",
                                      (int64_t)build_snapshot.diagnostic_count);
        }
        if (status != UMI_STATUS_OK) return status;
    }

    if (debugger != NULL &&
        umi_studio_debugger_service_snapshot(debugger, &debug_snapshot) ==
            UMI_STATUS_OK) {
        status = property_boolean(*out_view, "debugger-initialized",
                                  debug_snapshot.initialized);
        if (status == UMI_STATUS_OK) {
            status = property_integer(*out_view, "breakpoints",
                                      (int64_t)debug_snapshot.breakpoint_count);
        }
        if (status == UMI_STATUS_OK) {
            status = property_integer(*out_view, "debug-messages-received",
                                      (int64_t)debug_snapshot.received_messages);
        }
        if (status == UMI_STATUS_OK) {
            status = property_string(*out_view, "debug-state",
                                     debug_snapshot.controller_state);
        }
        if (status == UMI_STATUS_OK) {
            status = property_integer(*out_view, "debug-sessions",
                                      (int64_t)debug_snapshot.session_count);
        }
        if (status == UMI_STATUS_OK) {
            status = property_integer(*out_view, "debug-threads",
                                      (int64_t)debug_snapshot.thread_count);
        }
        if (status == UMI_STATUS_OK) {
            status = property_integer(*out_view, "debug-stack-frames",
                                      (int64_t)debug_snapshot.stack_frame_count);
        }
        if (status == UMI_STATUS_OK) {
            status = property_integer(*out_view, "debug-variables",
                                      (int64_t)debug_snapshot.variable_count);
        }
        if (status == UMI_STATUS_OK) {
            status = property_integer(*out_view, "debug-watches",
                                      (int64_t)debug_snapshot.watch_count);
        }
        if (status == UMI_STATUS_OK) {
            status = property_integer(*out_view, "debug-events",
                                      (int64_t)debug_snapshot.event_count);
        }
        if (status == UMI_STATUS_OK) {
            status = property_boolean(*out_view, "can-start",
                                      debug_snapshot.workspace.can_start);
        }
        if (status == UMI_STATUS_OK) {
            status = property_boolean(*out_view, "can-continue",
                                      debug_snapshot.workspace.can_continue);
        }
        if (status == UMI_STATUS_OK) {
            status = property_boolean(*out_view, "can-pause",
                                      debug_snapshot.workspace.can_pause);
        }
        if (status == UMI_STATUS_OK) {
            status = property_boolean(*out_view, "can-step",
                                      debug_snapshot.workspace.can_step);
        }
        if (status == UMI_STATUS_OK) {
            status = property_boolean(*out_view, "can-stop",
                                      debug_snapshot.workspace.can_stop);
        }
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 0U, "studio.action.build.configure",
                            "Configure", "Configure the active CMake profile");
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 1U, "studio.action.build.compile",
                            "Build", "Compile the active workspace");
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 2U, "studio.action.build.run",
                            "Start", "Start the configured Studio executable");
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 3U, "studio.action.build.install",
                            "Deploy", "Install into the local staging prefix");
    }
    if (status == UMI_STATUS_OK) {
        status = add_action_enabled(
            *out_view, 4U, "studio.action.debug.start", "Debug",
            "Start the configured debug target",
            debug_snapshot.workspace.can_start);
    }
    if (status == UMI_STATUS_OK) {
        status = add_action_enabled(
            *out_view, 5U, "studio.action.debug.continue", "Continue",
            "Continue the selected debug thread",
            debug_snapshot.workspace.can_continue);
    }
    if (status == UMI_STATUS_OK) {
        status = add_action_enabled(
            *out_view, 6U, "studio.action.debug.pause", "Pause",
            "Pause the selected debug thread",
            debug_snapshot.workspace.can_pause);
    }
    if (status == UMI_STATUS_OK) {
        status = add_action_enabled(
            *out_view, 7U, "studio.action.debug.next", "Step Over",
            "Step over the next statement", debug_snapshot.workspace.can_step);
    }
    if (status == UMI_STATUS_OK) {
        status = add_action_enabled(
            *out_view, 8U, "studio.action.debug.step-in", "Step Into",
            "Step into the next function call",
            debug_snapshot.workspace.can_step);
    }
    if (status == UMI_STATUS_OK) {
        status = add_action_enabled(
            *out_view, 9U, "studio.action.debug.step-out", "Step Out",
            "Step out of the current function",
            debug_snapshot.workspace.can_step);
    }
    if (status == UMI_STATUS_OK) {
        status = add_action_enabled(
            *out_view, 10U, "studio.action.debug.stop", "Stop",
            "Terminate the active debug session",
            debug_snapshot.workspace.can_stop);
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 11U,
                            "studio.action.debug.add-breakpoint",
                            "Breakpoint…", "Add a source breakpoint using path:line");
    }
    return status;
}

static UmiStatus create_debug_call_stack(const char *view_id,
                                         void *user_data,
                                         UmiUiViewModel **out_view)
{
    return umi_studio_debug_call_stack_view_create(
        view_id, umi_studio_services_debugger(
                     (UmiStudioServices *)user_data), out_view);
}

static UmiStatus create_debug_variables(const char *view_id,
                                        void *user_data,
                                        UmiUiViewModel **out_view)
{
    return umi_studio_debug_variables_view_create(
        view_id, umi_studio_services_debugger(
                     (UmiStudioServices *)user_data), out_view);
}

static UmiStatus create_debug_watches(const char *view_id,
                                      void *user_data,
                                      UmiUiViewModel **out_view)
{
    return umi_studio_debug_watches_view_create(
        view_id, umi_studio_services_debugger(
                     (UmiStudioServices *)user_data), out_view);
}

static UmiStatus create_debug_breakpoints(const char *view_id,
                                          void *user_data,
                                          UmiUiViewModel **out_view)
{
    return umi_studio_debug_breakpoints_view_create(
        view_id, umi_studio_services_debugger(
                     (UmiStudioServices *)user_data), out_view);
}

static UmiStatus create_debug_console(const char *view_id,
                                      void *user_data,
                                      UmiUiViewModel **out_view)
{
    return umi_studio_debug_console_view_create(
        view_id, umi_studio_services_debugger(
                     (UmiStudioServices *)user_data), out_view);
}

static UmiStatus create_testing(const char *view_id,
                                void *user_data,
                                UmiUiViewModel **out_view)
{
    UmiStudioTestService *service = umi_studio_services_tests(
        (UmiStudioServices *)user_data);
    return service != NULL
        ? umi_test_ui_explorer_view_create(
              view_id, umi_studio_test_service_workspace(service), out_view)
        : UMI_STATUS_UNAVAILABLE;
}

static UmiTestWorkspace *test_workspace(void *user_data)
{
    UmiStudioTestService *service = umi_studio_services_tests(
        (UmiStudioServices *)user_data);
    return service != NULL
        ? umi_studio_test_service_workspace(service) : NULL;
}

static UmiStatus create_test_results(const char *view_id, void *user_data,
                                     UmiUiViewModel **out_view)
{
    UmiTestWorkspace *workspace = test_workspace(user_data);
    return workspace != NULL
        ? umi_test_ui_results_view_create(view_id, workspace, out_view)
        : UMI_STATUS_UNAVAILABLE;
}

static UmiStatus create_test_failures(const char *view_id, void *user_data,
                                      UmiUiViewModel **out_view)
{
    UmiTestWorkspace *workspace = test_workspace(user_data);
    return workspace != NULL
        ? umi_test_ui_failures_view_create(view_id, workspace, out_view)
        : UMI_STATUS_UNAVAILABLE;
}

static UmiStatus create_test_output(const char *view_id, void *user_data,
                                    UmiUiViewModel **out_view)
{
    UmiTestWorkspace *workspace = test_workspace(user_data);
    return workspace != NULL
        ? umi_test_ui_output_view_create(view_id, workspace, out_view)
        : UMI_STATUS_UNAVAILABLE;
}

static UmiStatus create_test_coverage(const char *view_id, void *user_data,
                                      UmiUiViewModel **out_view)
{
    UmiTestWorkspace *workspace = test_workspace(user_data);
    return workspace != NULL
        ? umi_test_ui_coverage_view_create(view_id, workspace, out_view)
        : UMI_STATUS_UNAVAILABLE;
}

static UmiStatus create_test_runs(const char *view_id, void *user_data,
                                  UmiUiViewModel **out_view)
{
    UmiTestWorkspace *workspace = test_workspace(user_data);
    return workspace != NULL
        ? umi_test_ui_runs_view_create(view_id, workspace, out_view)
        : UMI_STATUS_UNAVAILABLE;
}

static UmiBuildWorkspace *build_workspace_for_view(void *user_data)
{
    UmiStudioBuildService *service = umi_studio_services_build(
        (UmiStudioServices *)user_data);
    return service != NULL
        ? umi_studio_build_service_workspace(service) : NULL;
}

#define DEFINE_BUILD_VIEW_FACTORY(name_, framework_factory_)                \
    static UmiStatus name_(const char *view_id, void *user_data,            \
                           UmiUiViewModel **out_view)                       \
    {                                                                       \
        UmiBuildWorkspace *workspace = build_workspace_for_view(user_data); \
        return workspace != NULL                                            \
            ? framework_factory_(view_id, workspace, out_view)              \
            : UMI_STATUS_UNAVAILABLE;                                       \
    }

DEFINE_BUILD_VIEW_FACTORY(create_build_dashboard,
                          umi_build_ui_dashboard_view_create)
DEFINE_BUILD_VIEW_FACTORY(create_build_graph,
                          umi_build_ui_graph_view_create)
DEFINE_BUILD_VIEW_FACTORY(create_build_history,
                          umi_build_ui_history_view_create)
DEFINE_BUILD_VIEW_FACTORY(create_build_output,
                          umi_build_ui_output_view_create)
DEFINE_BUILD_VIEW_FACTORY(create_build_artifacts,
                          umi_build_ui_artifacts_view_create)
DEFINE_BUILD_VIEW_FACTORY(create_build_tasks,
                          umi_build_ui_tasks_view_create)
#undef DEFINE_BUILD_VIEW_FACTORY

static UmiTradingWorkspace *trading_workspace_for_view(void *user_data)
{
    UmiStudioTradingService *service = umi_studio_services_trading(
        (UmiStudioServices *)user_data);
    return service != NULL
        ? umi_studio_trading_service_workspace(service) : NULL;
}

#define DEFINE_TRADING_VIEW_FACTORY(name_, framework_factory_)              \
    static UmiStatus name_(const char *view_id, void *user_data,             \
                           UmiUiViewModel **out_view)                        \
    {                                                                        \
        UmiTradingWorkspace *workspace =                                     \
            trading_workspace_for_view(user_data);                           \
        return workspace != NULL                                             \
            ? framework_factory_(view_id, workspace, out_view)               \
            : UMI_STATUS_UNAVAILABLE;                                        \
    }

DEFINE_TRADING_VIEW_FACTORY(create_trading_dashboard,
                            umi_trading_ui_dashboard_view_create)
DEFINE_TRADING_VIEW_FACTORY(create_trading_watchlist,
                            umi_trading_ui_watchlist_view_create)
DEFINE_TRADING_VIEW_FACTORY(create_trading_depth,
                            umi_trading_ui_depth_view_create)
DEFINE_TRADING_VIEW_FACTORY(create_trading_chart,
                            umi_trading_ui_chart_view_create)
DEFINE_TRADING_VIEW_FACTORY(create_trading_order_ticket,
                            umi_trading_ui_order_ticket_view_create)
DEFINE_TRADING_VIEW_FACTORY(create_trading_orders,
                            umi_trading_ui_orders_view_create)
DEFINE_TRADING_VIEW_FACTORY(create_trading_executions,
                            umi_trading_ui_executions_view_create)
DEFINE_TRADING_VIEW_FACTORY(create_trading_portfolio_risk,
                            umi_trading_ui_portfolio_risk_view_create)
#undef DEFINE_TRADING_VIEW_FACTORY

static UmiStatus create_output(const char *view_id,
                               void *user_data,
                               UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    return umi_diagnostic_output_view_create(
        view_id, umi_studio_services_diagnostic_pipeline(services), NULL,
        out_view);
}

static UmiStatus create_problems(const char *view_id,
                                 void *user_data,
                                 UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    return umi_diagnostic_problems_view_create(
        view_id, umi_studio_services_diagnostic_pipeline(services), out_view);
}

static UmiStatus create_terminal(const char *view_id,
                                 void *user_data,
                                 UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    return umi_terminal_ui_terminal_view_create(
        view_id, umi_studio_services_terminal_controller(services), out_view);
}

static UmiStatus create_processes(const char *view_id,
                                  void *user_data,
                                  UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    return umi_terminal_ui_process_view_create(
        view_id, umi_studio_services_terminal_controller(services), out_view);
}

static UmiStatus create_tasks(const char *view_id,
                              void *user_data,
                              UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    return umi_terminal_ui_task_view_create(
        view_id, umi_studio_services_terminal_controller(services), out_view);
}

static UmiStatus create_terminal_history(const char *view_id,
                                         void *user_data,
                                         UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    return umi_terminal_ui_history_view_create(
        view_id, umi_studio_services_terminal_controller(services), out_view);
}

static UmiStatus create_designer(const char *view_id,
                                 void *user_data,
                                 UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiStudioDesignerSnapshot snapshot;
    UmiStudioDesigner *designer;
    UmiStatus status = create_base_view(
        view_id,
        VIEW_DESIGNER,
        "Visual Designer",
        "Framework declarative designer state, selection and history.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    designer = umi_studio_services_designer(services);
    (void)memset(&snapshot, 0, sizeof(snapshot));

    if (designer == NULL ||
        umi_studio_designer_snapshot(designer, &snapshot) != UMI_STATUS_OK) {
        return property_boolean(*out_view, "available", 0);
    }

    status = property_boolean(*out_view, "available", 1);
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "palette-items",
                                  (int64_t)snapshot.palette_items);
    }
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "selected-items",
                                  (int64_t)snapshot.selected_items);
    }
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "undo",
                                  (int64_t)snapshot.undo_count);
    }
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "redo",
                                  (int64_t)snapshot.redo_count);
    }
    return status;
}

static UmiStatus create_applications(const char *view_id,
                                     void *user_data,
                                     UmiUiViewModel **out_view)
{
    (void)user_data;
    return umi_application_ui_portfolio_view_create(
        view_id, "org.umicom.studio", out_view);
}

static UmiStatus create_application_components(const char *view_id,
                                                void *user_data,
                                                UmiUiViewModel **out_view)
{
    (void)user_data;
    return umi_application_ui_component_catalogue_view_create(
        view_id, "development", out_view);
}

static UmiStatus create_gtk4_coverage(const char *view_id,
                                      void *user_data,
                                      UmiUiViewModel **out_view)
{
    (void)user_data;
    return umi_application_ui_gtk4_coverage_view_create(view_id, out_view);
}

static UmiStatus create_architecture_audit(const char *view_id,
                                           void *user_data,
                                           UmiUiViewModel **out_view)
{
    (void)user_data;
    return umi_application_ui_boundary_audit_view_create(view_id, out_view);
}

static UmiStatus create_framework(const char *view_id,
                                  void *user_data,
                                  UmiUiViewModel **out_view)
{
    UmiStatus status;
    (void)user_data;

    status = create_base_view(
        view_id,
        VIEW_FRAMEWORK,
        "Umicom Framework",
        "Reusable C23 platform capabilities consumed by Studio and other products.",
        out_view);
    if (status == UMI_STATUS_OK) {
        status = property_string(*out_view, "version",
                                 UMICOM_FRAMEWORK_VERSION_STRING);
    }
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "abi",
                                  (int64_t)UMICOM_FRAMEWORK_ABI_VERSION);
    }
    if (status == UMI_STATUS_OK) {
        status = property_string(*out_view, "language", "C23");
    }
    if (status == UMI_STATUS_OK) {
        status = property_string(*out_view, "ui-contract",
                                 "toolkit-neutral + GTK4 reference adapter");
    }
    return status;
}

static UmiStatus create_ai(const char *view_id,
                           void *user_data,
                           UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiStudioAiPlatform *platform = umi_studio_services_ai_platform(services);
    UmiStatus status;
    if (platform == NULL) return UMI_STATUS_INVALID_STATE;
    status = umi_ai_ui_authorengine_overview_view_create(
        view_id, umi_studio_ai_platform_authorengine(platform), out_view);
    /* Preserve the original Batch 23 properties for existing adapters while
     * adding the richer Integration v2 view model around them. */
    if (status == UMI_STATUS_OK) {
        status = property_boolean(*out_view, "available", 1);
    }
    if (status == UMI_STATUS_OK) {
        status = property_string(
            *out_view, "default-provider",
            umi_studio_ai_platform_default_provider(platform));
    }
    if (status == UMI_STATUS_OK) {
        status = property_boolean(
            *out_view, "helix-runtime",
            umi_studio_ai_platform_helix(platform) != NULL);
    }
    return status;
}

static UmiStatus create_ai_runtimes(const char *view_id,
                                    void *user_data,
                                    UmiUiViewModel **out_view)
{
    UmiStudioAiPlatform *platform = umi_studio_services_ai_platform(
        (UmiStudioServices *)user_data);
    return platform != NULL
        ? umi_ai_ui_runtime_catalogue_view_create(
              view_id, umi_studio_ai_platform_authorengine(platform), out_view)
        : UMI_STATUS_INVALID_STATE;
}

static UmiStatus create_ai_context(const char *view_id,
                                   void *user_data,
                                   UmiUiViewModel **out_view)
{
    UmiStudioAiPlatform *platform = umi_studio_services_ai_platform(
        (UmiStudioServices *)user_data);
    return platform != NULL
        ? umi_ai_ui_context_view_create(
              view_id, umi_studio_ai_platform_authorengine(platform), out_view)
        : UMI_STATUS_INVALID_STATE;
}

static UmiStatus create_ai_sessions(const char *view_id,
                                    void *user_data,
                                    UmiUiViewModel **out_view)
{
    UmiStudioAiPlatform *platform = umi_studio_services_ai_platform(
        (UmiStudioServices *)user_data);
    return platform != NULL
        ? umi_ai_ui_sessions_view_create(
              view_id, umi_studio_ai_platform_authorengine(platform), out_view)
        : UMI_STATUS_INVALID_STATE;
}

static UmiStatus create_ai_privacy(const char *view_id,
                                   void *user_data,
                                   UmiUiViewModel **out_view)
{
    UmiStudioAiPlatform *platform = umi_studio_services_ai_platform(
        (UmiStudioServices *)user_data);
    return platform != NULL
        ? umi_ai_ui_privacy_view_create(
              view_id, umi_studio_ai_platform_authorengine(platform), out_view)
        : UMI_STATUS_INVALID_STATE;
}

static UmiStatus create_ai_coding(const char *view_id,
                                  void *user_data,
                                  UmiUiViewModel **out_view)
{
    UmiStudioAiPlatform *platform = umi_studio_services_ai_platform(
        (UmiStudioServices *)user_data);
    return platform != NULL
        ? umi_ai_ui_coding_assistant_view_create(
              view_id, umi_studio_ai_platform_coding_assistant(platform),
              out_view)
        : UMI_STATUS_INVALID_STATE;
}

static UmiStatus create_ai_coding_context(const char *view_id,
                                          void *user_data,
                                          UmiUiViewModel **out_view)
{
    UmiStudioAiPlatform *platform = umi_studio_services_ai_platform(
        (UmiStudioServices *)user_data);
    return platform != NULL
        ? umi_ai_ui_coding_repository_view_create(
              view_id, umi_studio_ai_platform_coding_assistant(platform),
              out_view)
        : UMI_STATUS_INVALID_STATE;
}

static UmiStatus create_ai_patch_review(const char *view_id,
                                        void *user_data,
                                        UmiUiViewModel **out_view)
{
    UmiStudioAiPlatform *platform = umi_studio_services_ai_platform(
        (UmiStudioServices *)user_data);
    UmiAiCodingAssistantSnapshot snapshot;
    UmiStatus status;
    if (platform == NULL) return UMI_STATUS_INVALID_STATE;
    status = umi_ai_coding_assistant_snapshot(
        umi_studio_ai_platform_coding_assistant(platform), &snapshot);
    return status == UMI_STATUS_OK
        ? umi_ai_ui_coding_patch_view_create(
              view_id, umi_studio_ai_platform_coding_assistant(platform),
              snapshot.last_patch_id, out_view)
        : status;
}

static UmiStatus create_knowledge(const char *view_id,
                                  void *user_data,
                                  UmiUiViewModel **out_view)
{
    return umi_studio_knowledge_overview_view(
        umi_studio_services_ai_platform((UmiStudioServices *)user_data),
        view_id, out_view);
}

static UmiStatus create_knowledge_collections(
    const char *view_id, void *user_data, UmiUiViewModel **out_view)
{
    return umi_studio_knowledge_collections_view(
        umi_studio_services_ai_platform((UmiStudioServices *)user_data),
        view_id, out_view);
}

static UmiStatus create_knowledge_sources(
    const char *view_id, void *user_data, UmiUiViewModel **out_view)
{
    return umi_studio_knowledge_sources_view(
        umi_studio_services_ai_platform((UmiStudioServices *)user_data),
        view_id, out_view);
}

static UmiStatus create_knowledge_search(
    const char *view_id, void *user_data, UmiUiViewModel **out_view)
{
    return umi_studio_knowledge_search_view(
        umi_studio_services_ai_platform((UmiStudioServices *)user_data),
        view_id, "Umicom Framework architecture", out_view);
}

static UmiStatus create_knowledge_source(
    const char *view_id, void *user_data, UmiUiViewModel **out_view)
{
    UmiKnowledgeMatch match = {0};
    (void)user_data;
    (void)snprintf(match.citation.source_id,
                   sizeof(match.citation.source_id), "%s", "none");
    (void)snprintf(match.citation.title,
                   sizeof(match.citation.title), "%s", "Select a search result");
    (void)snprintf(match.citation.uri,
                   sizeof(match.citation.uri), "%s", "knowledge://selection");
    (void)snprintf(match.chunk.text, sizeof(match.chunk.text), "%s",
                   "The cited source range will appear here.");
    return umi_studio_knowledge_source_view(view_id, &match, out_view);
}

typedef struct StudioViewDefinition {
    const char *view_type;
    StudioViewCreateFn create;
} StudioViewDefinition;

static const StudioViewDefinition DEFINITIONS[] = {
    { VIEW_EXPLORER, create_explorer },
    { VIEW_SEARCH, create_search },
    { VIEW_SOURCE_CTRL, create_source_control },
    { VIEW_VCS_COMMIT, create_vcs_commit },
    { VIEW_VCS_HISTORY, create_vcs_history },
    { VIEW_VCS_BRANCHES, create_vcs_branches },
    { VIEW_VCS_REMOTES, create_vcs_remotes },
    { VIEW_VCS_CONFLICTS, create_vcs_conflicts },
    { VIEW_VCS_DIFF, create_vcs_diff },
    { VIEW_VCS_OPERATIONS, create_vcs_operations },
    { VIEW_RUN_DEBUG, create_run_debug },
    { VIEW_DEBUG_CALL_STACK, create_debug_call_stack },
    { VIEW_DEBUG_VARIABLES, create_debug_variables },
    { VIEW_DEBUG_WATCHES, create_debug_watches },
    { VIEW_DEBUG_BREAKPOINTS, create_debug_breakpoints },
    { VIEW_DEBUG_CONSOLE, create_debug_console },
    { VIEW_TESTING, create_testing },
    { VIEW_TEST_RESULTS, create_test_results },
    { VIEW_TEST_FAILURES, create_test_failures },
    { VIEW_TEST_OUTPUT, create_test_output },
    { VIEW_TEST_COVERAGE, create_test_coverage },
    { VIEW_TEST_RUNS, create_test_runs },
    { VIEW_BUILD_DASHBOARD, create_build_dashboard },
    { VIEW_BUILD_GRAPH, create_build_graph },
    { VIEW_BUILD_HISTORY, create_build_history },
    { VIEW_BUILD_OUTPUT, create_build_output },
    { VIEW_BUILD_ARTIFACTS, create_build_artifacts },
    { VIEW_BUILD_TASKS, create_build_tasks },
    { VIEW_TRADING_DASHBOARD, create_trading_dashboard },
    { VIEW_TRADING_WATCHLIST, create_trading_watchlist },
    { VIEW_TRADING_DEPTH, create_trading_depth },
    { VIEW_TRADING_CHART, create_trading_chart },
    { VIEW_TRADING_ORDER_TICKET, create_trading_order_ticket },
    { VIEW_TRADING_ORDERS, create_trading_orders },
    { VIEW_TRADING_EXECUTIONS, create_trading_executions },
    { VIEW_TRADING_PORTFOLIO_RISK, create_trading_portfolio_risk },
    { VIEW_DESIGNER, create_designer },
    { VIEW_APPLICATIONS, create_applications },
    { VIEW_APPLICATION_COMPONENTS, create_application_components },
    { VIEW_GTK4_COVERAGE, create_gtk4_coverage },
    { VIEW_ARCHITECTURE, create_architecture_audit },
    { VIEW_FRAMEWORK, create_framework },
    { VIEW_AI, create_ai },
    { VIEW_AI_RUNTIMES, create_ai_runtimes },
    { VIEW_AI_CONTEXT, create_ai_context },
    { VIEW_AI_SESSIONS, create_ai_sessions },
    { VIEW_AI_PRIVACY, create_ai_privacy },
    { VIEW_AI_CODING, create_ai_coding },
    { VIEW_AI_CODING_CONTEXT, create_ai_coding_context },
    { VIEW_AI_PATCH_REVIEW, create_ai_patch_review },
    { VIEW_KNOWLEDGE, create_knowledge },
    { VIEW_KNOWLEDGE_COLLECTIONS, create_knowledge_collections },
    { VIEW_KNOWLEDGE_SOURCES, create_knowledge_sources },
    { VIEW_KNOWLEDGE_SEARCH, create_knowledge_search },
    { VIEW_KNOWLEDGE_SOURCE, create_knowledge_source },
    { VIEW_OUTPUT, create_output },
    { VIEW_PROBLEMS, create_problems },
    { VIEW_TERMINAL, create_terminal },
    { VIEW_PROCESSES, create_processes },
    { VIEW_TASKS, create_tasks },
    { VIEW_HISTORY, create_terminal_history },
    { VIEW_EXTENSIONS, create_extensions },
    { VIEW_EXT_CATALOGUE, create_extension_catalogue },
    { VIEW_EXT_PERMISSIONS, create_extension_permissions },
    { VIEW_EXT_AUDIT, create_extension_audit },
    { VIEW_PRODUCT_MARKETPLACE, create_product_marketplace },
    { VIEW_PRODUCTS_INSTALLED, create_products_installed },
    { VIEW_PRODUCT_UPDATES, create_product_updates },
    { VIEW_PRODUCT_TRANSACTIONS, create_product_transactions },
    { VIEW_PRODUCT_EVIDENCE, create_product_evidence }
};

UmiStatus umi_studio_workbench_views_register(
    UmiUiWorkbench *workbench,
    UmiStudioServices *services)
{
    if (workbench == NULL || services == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_studio_workbench_views_register_registry(
        umi_ui_workbench_view_factories(workbench), services);
}

UmiStatus umi_studio_workbench_views_register_registry(
    UmiUiViewFactoryRegistry *registry,
    UmiStudioServices *services)
{
    size_t index;
    if (registry == NULL || services == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < sizeof(DEFINITIONS) / sizeof(DEFINITIONS[0]);
         ++index) {
        UmiUiViewFactoryDescriptor descriptor = {0};
        UmiStatus status;

        (void)snprintf(descriptor.view_type, sizeof(descriptor.view_type),
                       "%s", DEFINITIONS[index].view_type);
        (void)snprintf(descriptor.provider_id, sizeof(descriptor.provider_id),
                       "%s", "org.umicom.studio.workbench");
        descriptor.create = DEFINITIONS[index].create;
        descriptor.user_data = services;

        status = umi_ui_view_factory_register(registry, &descriptor);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

size_t umi_studio_workbench_view_definition_count(void)
{
    return sizeof(DEFINITIONS) / sizeof(DEFINITIONS[0]);
}
