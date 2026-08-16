/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_build_workspace_views.c
 *
 * PURPOSE:
 *   Verify Studio composes every professional build-and-task pane from the
 *   single Framework workspace owned by its existing build service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/studio/bootstrap.h"
#include "umicom/studio/build.h"
#include "umicom/studio/contributions.h"
#include "umicom/ui/command_view.h"

static void add_evidence(UmiStudioBuildService *build)
{
    UmiBuildResult *result = NULL;
    UmiBuildArtifactSnapshot artifact = {0};

    assert(umi_build_result_create(&result) == UMI_STATUS_OK);
    umi_build_result_init(result, 75U, UMI_BUILD_PHASE_BUILD,
                          "studio.development");
    (void)strcpy(result->command,
                 "cmake --build build/windows-ucrt64-debug --parallel 2");
    (void)strcpy(result->output, "Build completed successfully");
    umi_build_result_finish(result, UMI_STATUS_OK, 0, 250U);
    assert(umi_build_history_append(
               umi_studio_build_service_history(build), result) ==
           UMI_STATUS_OK);

    artifact.operation_id = 75U;
    (void)strcpy(artifact.node_id, "build");
    (void)strcpy(artifact.artifact_id, "studio.executable");
    (void)strcpy(artifact.path,
                 "build/windows-ucrt64-debug/bin/umicom-studio-ide.exe");
    (void)strcpy(artifact.kind, "executable");
    artifact.size_bytes = 8192U;
    assert(umi_studio_build_service_record_artifact(build, &artifact) ==
           UMI_STATUS_OK);
    umi_build_result_destroy(result);
}

static void verify_view(UmiUiWorkbench *workbench, const char *view_type,
                        const char *pane_id, const char *expected_kind)
{
    UmiUiViewModel *view = NULL;
    UmiUiValue kind;

    assert(umi_ui_view_factory_create_view(
               umi_ui_workbench_view_factories(workbench), view_type, pane_id,
               &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
               view, "umicom.view-kind", &kind) == UMI_STATUS_OK);
    assert(kind.kind == UMI_UI_VALUE_STRING);
    assert(strcmp(kind.string_value, expected_kind) == 0);
    umi_ui_view_model_destroy(view);
}

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioServices *services;
    UmiStudioBuildService *build;
    UmiUiWorkbench *workbench;
    UmiUiViewModel *dashboard = NULL;
    UmiUiCommandViewAction action;

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    services = umi_studio_bootstrap_services(bootstrap);
    build = umi_studio_services_build(services);
    assert(build != NULL);
    add_evidence(build);
    workbench = umi_studio_ui_workbench(umi_studio_bootstrap_ui(bootstrap));

    verify_view(workbench, "studio.build-dashboard",
                UMI_STUDIO_PANE_BUILD_DASHBOARD, "build-dashboard");
    verify_view(workbench, "studio.build-graph", UMI_STUDIO_PANE_BUILD_GRAPH,
                "build-graph");
    verify_view(workbench, "studio.build-history",
                UMI_STUDIO_PANE_BUILD_HISTORY, "build-history");
    verify_view(workbench, "studio.build-output",
                UMI_STUDIO_PANE_BUILD_OUTPUT, "build-output");
    verify_view(workbench, "studio.build-artifacts",
                UMI_STUDIO_PANE_BUILD_ARTIFACTS, "build-artifacts");
    verify_view(workbench, "studio.build-tasks",
                UMI_STUDIO_PANE_BUILD_TASKS, "build-tasks");

    assert(umi_ui_view_factory_create_view(
               umi_ui_workbench_view_factories(workbench),
               "studio.build-dashboard", UMI_STUDIO_PANE_BUILD_DASHBOARD,
               &dashboard) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(dashboard, 6U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id, "studio.action.build.run-all") == 0);
    assert(action.enabled);
    assert(umi_ui_command_view_action_at(dashboard, 7U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id, "studio.action.build.cancel") == 0);
    assert(!action.enabled);
    umi_ui_view_model_destroy(dashboard);

    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}
