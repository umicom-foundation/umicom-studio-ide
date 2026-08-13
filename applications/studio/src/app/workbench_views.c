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
#include "umicom/studio/designer.h"
#include "umicom/studio/language.h"
#include "umicom/studio/source_control.h"
#include "umicom/studio/tests.h"
#include "umicom/studio/terminal.h"

#define VIEW_EXPLORER      "studio.project-explorer"
#define VIEW_SEARCH        "studio.search"
#define VIEW_SOURCE_CTRL   "studio.source-control"
#define VIEW_VCS_HISTORY   "studio.vcs-history"
#define VIEW_VCS_BRANCHES  "studio.vcs-branches"
#define VIEW_VCS_REMOTES   "studio.vcs-remotes"
#define VIEW_VCS_DIFF      "studio.vcs-diff"
#define VIEW_RUN_DEBUG     "studio.run-debug"
#define VIEW_TESTING       "studio.testing"
#define VIEW_DESIGNER      "studio.designer"
#define VIEW_APPLICATIONS  "studio.application-hub"
#define VIEW_FRAMEWORK     "studio.framework"
#define VIEW_AI            "studio.authorengine"
#define VIEW_OUTPUT        "studio.output"
#define VIEW_PROBLEMS      "studio.problems"
#define VIEW_TERMINAL      "studio.terminal"
#define VIEW_PROCESSES     "studio.processes"
#define VIEW_TASKS         "studio.tasks"
#define VIEW_HISTORY       "studio.terminal-history"

static UmiStatus add_action(UmiUiViewModel *view,
                            size_t index,
                            const char *action_id,
                            const char *label,
                            const char *tooltip)
{
    UmiUiCommandViewAction action = {0};
    (void)snprintf(action.action_id, sizeof(action.action_id), "%s", action_id);
    (void)snprintf(action.label, sizeof(action.label), "%s", label);
    (void)snprintf(action.tooltip, sizeof(action.tooltip), "%s", tooltip);
    action.enabled = 1;
    return umi_ui_command_view_set_action(view, index, &action);
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
        ? umi_vcs_ui_source_control_view_create(
              view_id,
              umi_studio_source_control_service_workspace(service),
              out_view)
        : UMI_STATUS_UNAVAILABLE;
}

static UmiVcsWorkspace *source_control_workspace(void *user_data)
{
    UmiStudioSourceControlService *service = umi_studio_services_source_control(
        (UmiStudioServices *)user_data);
    return service != NULL
        ? umi_studio_source_control_service_workspace(service) : NULL;
}

static UmiStatus create_vcs_history(const char *view_id, void *user_data, UmiUiViewModel **out_view)
{ UmiVcsWorkspace *workspace = source_control_workspace(user_data); return workspace != NULL ? umi_vcs_ui_history_view_create(view_id, workspace, out_view) : UMI_STATUS_UNAVAILABLE; }
static UmiStatus create_vcs_branches(const char *view_id, void *user_data, UmiUiViewModel **out_view)
{ UmiVcsWorkspace *workspace = source_control_workspace(user_data); return workspace != NULL ? umi_vcs_ui_branches_view_create(view_id, workspace, out_view) : UMI_STATUS_UNAVAILABLE; }
static UmiStatus create_vcs_remotes(const char *view_id, void *user_data, UmiUiViewModel **out_view)
{ UmiVcsWorkspace *workspace = source_control_workspace(user_data); return workspace != NULL ? umi_vcs_ui_remotes_view_create(view_id, workspace, out_view) : UMI_STATUS_UNAVAILABLE; }
static UmiStatus create_vcs_diff(const char *view_id, void *user_data, UmiUiViewModel **out_view)
{ UmiVcsWorkspace *workspace = source_control_workspace(user_data); return workspace != NULL ? umi_vcs_ui_diff_view_create(view_id, workspace, out_view) : UMI_STATUS_UNAVAILABLE; }

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
        status = add_action(*out_view, 4U, "studio.action.debug.start",
                            "Debug", "Start the configured debug target");
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 5U, "studio.action.debug.continue",
                            "Continue", "Continue the selected debug thread");
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 6U, "studio.action.debug.pause",
                            "Pause", "Pause the selected debug thread");
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 7U, "studio.action.debug.next",
                            "Step Over", "Step over the next statement");
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 8U, "studio.action.debug.step-in",
                            "Step Into", "Step into the next function call");
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 9U, "studio.action.debug.step-out",
                            "Step Out", "Step out of the current function");
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 10U, "studio.action.debug.stop",
                            "Stop", "Terminate the active debug session");
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 11U,
                            "studio.action.debug.add-breakpoint",
                            "Breakpoint…", "Add a source breakpoint using path:line");
    }
    return status;
}

static UmiStatus create_testing(const char *view_id,
                                void *user_data,
                                UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiStudioTestSnapshot snapshot;
    UmiStudioTestService *service;
    UmiStatus status = create_base_view(
        view_id,
        VIEW_TESTING,
        "Testing",
        "Discovered test suites and latest Framework testing summary.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    service = umi_studio_services_tests(services);
    (void)memset(&snapshot, 0, sizeof(snapshot));

    if (service == NULL ||
        umi_studio_test_service_snapshot(service, &snapshot) != UMI_STATUS_OK) {
        return property_boolean(*out_view, "available", 0);
    }

    status = property_boolean(*out_view, "available", 1);
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "suites",
                                  (int64_t)snapshot.suite_count);
    }
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "tests",
                                  (int64_t)snapshot.test_count);
    }
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "passed",
                                  (int64_t)snapshot.passed);
    }
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "failed",
                                  (int64_t)snapshot.failed);
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 0U, "studio.action.test.discover",
                            "Discover", "Discover CTest tests in the build directory");
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 1U, "studio.action.build.test",
                            "Run All", "Run the complete CTest suite");
    }
    return status;
}

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
    UmiStatus status;
    (void)user_data;

    status = create_base_view(
        view_id,
        VIEW_APPLICATIONS,
        "Applications",
        "Umicom application catalogue, runtime presence and safe launch planning.",
        out_view);
    if (status == UMI_STATUS_OK) {
        status = property_string(*out_view, "provider",
                                 "Studio Runtime Manager / Integration Fabric");
    }
    if (status == UMI_STATUS_OK) {
        status = property_string(*out_view, "launch-mode",
                                 "planned and supervised");
    }
    return status;
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
    UmiStudioAiPlatform *platform;
    const char *provider;
    UmiStatus status = create_base_view(
        view_id,
        VIEW_AI,
        "AI / AuthorEngine",
        "Studio AI composition over the reusable Framework AI and Helix runtimes.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    platform = umi_studio_services_ai_platform(services);
    provider = platform != NULL
        ? umi_studio_ai_platform_default_provider(platform)
        : NULL;

    status = property_boolean(*out_view, "available", platform != NULL);
    if (status == UMI_STATUS_OK) {
        status = property_string(*out_view, "default-provider",
                                 provider != NULL ? provider : "not configured");
    }
    if (status == UMI_STATUS_OK) {
        status = property_boolean(*out_view, "helix-runtime",
                                  platform != NULL &&
                                  umi_studio_ai_platform_helix(platform) != NULL);
    }
    return status;
}

typedef struct StudioViewDefinition {
    const char *view_type;
    StudioViewCreateFn create;
} StudioViewDefinition;

static const StudioViewDefinition DEFINITIONS[] = {
    { VIEW_EXPLORER, create_explorer },
    { VIEW_SEARCH, create_search },
    { VIEW_SOURCE_CTRL, create_source_control },
    { VIEW_VCS_HISTORY, create_vcs_history },
    { VIEW_VCS_BRANCHES, create_vcs_branches },
    { VIEW_VCS_REMOTES, create_vcs_remotes },
    { VIEW_VCS_DIFF, create_vcs_diff },
    { VIEW_RUN_DEBUG, create_run_debug },
    { VIEW_TESTING, create_testing },
    { VIEW_DESIGNER, create_designer },
    { VIEW_APPLICATIONS, create_applications },
    { VIEW_FRAMEWORK, create_framework },
    { VIEW_AI, create_ai },
    { VIEW_OUTPUT, create_output },
    { VIEW_PROBLEMS, create_problems },
    { VIEW_TERMINAL, create_terminal },
    { VIEW_PROCESSES, create_processes },
    { VIEW_TASKS, create_tasks },
    { VIEW_HISTORY, create_terminal_history }
};

UmiStatus umi_studio_workbench_views_register(
    UmiUiWorkbench *workbench,
    UmiStudioServices *services)
{
    UmiUiViewFactoryRegistry *registry;
    size_t index;

    if (workbench == NULL || services == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    registry = umi_ui_workbench_view_factories(workbench);
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
