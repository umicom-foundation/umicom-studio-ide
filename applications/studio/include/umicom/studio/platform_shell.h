/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/platform_shell.h
 *
 * PURPOSE:
 *   Define the Studio composition surface over reusable Framework workbench, resource, product, chart, designer and frontend platform services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio composes reusable Framework capabilities here. Generic models and
 * engines stay in Framework so the same mechanisms can serve future products.
 */
#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_PLATFORM_SHELL_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_PLATFORM_SHELL_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/umicom.h"
#include "umicom/studio/services.h"
#include "umicom/studio/developer_workbench.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioPlatformShell UmiStudioPlatformShell;
typedef struct UmiStudioPlatformShellSnapshot {uint32_t struct_size;uint32_t api_version;char area_id[128];char view_type[128];char title[256];char summary[512];uint64_t revision;size_t item_count;int available;UmiUiWorkbenchPlatformSnapshot workbench;UmiResourceCentreSnapshot resources;UmiProductCentreSnapshot products;UmiChartWorkspaceSnapshot charts;UmiDesignerAuthoringSessionSnapshot designer;UmiFrontendApplicationSnapshot frontend;UmiStudioDeveloperWorkbenchSnapshot developer;} UmiStudioPlatformShellSnapshot;
UmiStatus umi_studio_platform_shell_create(UmiStudioServices *services,UmiStudioPlatformShell **out_shell);
void umi_studio_platform_shell_destroy(UmiStudioPlatformShell *shell);
UmiStatus umi_studio_platform_shell_snapshot(UmiStudioPlatformShell *shell,UmiStudioPlatformShellSnapshot *out_snapshot);
UmiUiWorkbenchPlatform *umi_studio_platform_shell_workbench(UmiStudioPlatformShell *shell);
UmiResourceCentre *umi_studio_platform_shell_resources(UmiStudioPlatformShell *shell);
UmiProductCentre *umi_studio_platform_shell_products(UmiStudioPlatformShell *shell);
UmiChartWorkspace *umi_studio_platform_shell_charts(UmiStudioPlatformShell *shell);
UmiDesignerAuthoringSession *umi_studio_platform_shell_designer(UmiStudioPlatformShell *shell);
UmiFrontendApplication *umi_studio_platform_shell_frontend(UmiStudioPlatformShell *shell);
UmiStudioDeveloperWorkbench *umi_studio_platform_shell_developer(UmiStudioPlatformShell *shell);

UmiStatus umi_studio_platform_shell_activate_project(
    UmiStudioPlatformShell *shell,
    const UmiProjectWorkspaceSelectionRequest *request,
    UmiProjectWorkspaceSelectionSnapshot *out_selection);

UmiStatus umi_studio_platform_shell_validate_project(
    UmiStudioPlatformShell *shell,
    UmiProjectWorkspaceValidationReport *out_report);

UmiStatus umi_studio_platform_shell_prepare_project_workflow(
    UmiStudioPlatformShell *shell,
    const UmiDeveloperProjectWorkflowRequest *request,
    UmiDeveloperProjectWorkflowSnapshot *out_workflow);

UmiStatus umi_studio_platform_shell_import_project(
    UmiStudioPlatformShell *shell,
    const UmiDeveloperProjectBootstrapRequest *request,
    UmiDeveloperProjectBootstrapSnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif
#endif
