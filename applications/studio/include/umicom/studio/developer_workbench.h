/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/developer_workbench.h
 * PURPOSE: Aggregate the complete Studio developer platform over Framework services.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_DEVELOPER_WORKBENCH_H
#define UMICOM_STUDIO_DEVELOPER_WORKBENCH_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/studio/project_centre.h"
#include "umicom/studio/language_intelligence_centre.h"
#include "umicom/studio/debug_centre.h"
#include "umicom/studio/source_control_centre.h"
#include "umicom/studio/test_explorer_centre.h"
#include "umicom/studio/problems_centre.h"
#include "umicom/studio/output_centre.h"
#include "umicom/studio/progress_centre.h"
#include "umicom/studio/developer_session.h"
#include "umicom/studio/developer_pipeline_centre.h"
#include "umicom/studio/developer_task_centre.h"
#include "umicom/studio/developer_run_centre.h"
#include "umicom/studio/developer_workspace_state_centre.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioDeveloperWorkbench UmiStudioDeveloperWorkbench;
typedef struct UmiStudioDeveloperWorkbenchSnapshot {uint32_t struct_size;uint32_t api_version;UmiStudioProjectCentreSnapshot projects;UmiStudioLanguageIntelligenceCentreSnapshot language;UmiStudioDebugCentreSnapshot debug;UmiStudioSourceControlCentreSnapshot source_control;UmiStudioTestExplorerCentreSnapshot tests;UmiStudioProblemsCentreSnapshot problems;UmiStudioOutputCentreSnapshot output;UmiStudioProgressCentreSnapshot progress;UmiStudioDeveloperSessionSnapshot session;uint64_t revision;size_t item_count;int available;UmiStudioDeveloperPipelineCentreSnapshot pipeline;UmiStudioDeveloperTaskCentreSnapshot tasks;UmiStudioDeveloperRunCentreSnapshot run;UmiStudioDeveloperWorkspaceStateCentreSnapshot workspace_state;} UmiStudioDeveloperWorkbenchSnapshot;
UmiStatus umi_studio_developer_workbench_create(UmiStudioDeveloperWorkbench **out_workbench);
void umi_studio_developer_workbench_destroy(UmiStudioDeveloperWorkbench *workbench);
UmiStatus umi_studio_developer_workbench_snapshot(UmiStudioDeveloperWorkbench *workbench,UmiStudioDeveloperWorkbenchSnapshot *out_snapshot);
UmiStudioProjectCentre *umi_studio_developer_workbench_projects(UmiStudioDeveloperWorkbench *workbench);
UmiStudioLanguageIntelligenceCentre *umi_studio_developer_workbench_language(UmiStudioDeveloperWorkbench *workbench);
UmiStudioDebugCentre *umi_studio_developer_workbench_debug(UmiStudioDeveloperWorkbench *workbench);
UmiStudioSourceControlCentre *umi_studio_developer_workbench_source_control(UmiStudioDeveloperWorkbench *workbench);
UmiStudioTestExplorerCentre *umi_studio_developer_workbench_tests(UmiStudioDeveloperWorkbench *workbench);
UmiUiWorkbenchServices *umi_studio_developer_workbench_services(UmiStudioDeveloperWorkbench *workbench);
UmiDeveloperRuntime *umi_studio_developer_workbench_runtime(UmiStudioDeveloperWorkbench *workbench);
UmiDeveloperUniversalModel *umi_studio_developer_workbench_universal_model(
    UmiStudioDeveloperWorkbench *workbench);
UmiStudioDeveloperPipelineCentre *umi_studio_developer_workbench_pipeline(UmiStudioDeveloperWorkbench *workbench);
UmiStudioDeveloperTaskCentre *umi_studio_developer_workbench_task_centre(UmiStudioDeveloperWorkbench *workbench);
UmiStudioDeveloperRunCentre *umi_studio_developer_workbench_run_centre(UmiStudioDeveloperWorkbench *workbench);
UmiStudioDeveloperWorkspaceStateCentre *umi_studio_developer_workbench_workspace_state(UmiStudioDeveloperWorkbench *workbench);
UmiStudioDeveloperSession *umi_studio_developer_workbench_session(UmiStudioDeveloperWorkbench *workbench);

UmiStatus umi_studio_developer_workbench_activate_project(
    UmiStudioDeveloperWorkbench *workbench,
    const UmiProjectWorkspaceSelectionRequest *request,
    UmiProjectWorkspaceSelectionSnapshot *out_selection);

UmiStatus umi_studio_developer_workbench_prepare_project_workflow(
    UmiStudioDeveloperWorkbench *workbench,
    const UmiDeveloperProjectWorkflowRequest *request,
    UmiDeveloperProjectWorkflowSnapshot *out_workflow);

UmiStatus umi_studio_developer_workbench_validate_project(
    UmiStudioDeveloperWorkbench *workbench,
    UmiProjectWorkspaceValidationReport *out_report);

UmiStatus umi_studio_developer_workbench_import_project(
    UmiStudioDeveloperWorkbench *workbench,
    const UmiDeveloperProjectBootstrapRequest *request,
    UmiDeveloperProjectBootstrapSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
