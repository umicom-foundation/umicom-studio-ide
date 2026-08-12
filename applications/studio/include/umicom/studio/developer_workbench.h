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
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioDeveloperWorkbench UmiStudioDeveloperWorkbench;
typedef struct UmiStudioDeveloperWorkbenchSnapshot {uint32_t struct_size;uint32_t api_version;UmiStudioProjectCentreSnapshot projects;UmiStudioLanguageIntelligenceCentreSnapshot language;UmiStudioDebugCentreSnapshot debug;UmiStudioSourceControlCentreSnapshot source_control;UmiStudioTestExplorerCentreSnapshot tests;UmiStudioProblemsCentreSnapshot problems;UmiStudioOutputCentreSnapshot output;UmiStudioProgressCentreSnapshot progress;UmiStudioDeveloperSessionSnapshot session;uint64_t revision;size_t item_count;int available;UmiStudioDeveloperPipelineCentreSnapshot pipeline;} UmiStudioDeveloperWorkbenchSnapshot;
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
UmiStudioDeveloperPipelineCentre *umi_studio_developer_workbench_pipeline(UmiStudioDeveloperWorkbench *workbench);
UmiStudioDeveloperSession *umi_studio_developer_workbench_session(UmiStudioDeveloperWorkbench *workbench);
#ifdef __cplusplus
}
#endif
#endif
