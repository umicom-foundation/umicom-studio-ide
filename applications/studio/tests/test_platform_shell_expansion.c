/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_platform_shell_expansion.c
 *
 * PURPOSE:
 *   Exercise the major Studio platform shell and all new product-composition centre snapshots.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio composes reusable Framework capabilities here. Generic models and
 * engines stay in Framework so the same mechanisms can serve future products.
 */
#include <string.h>
#include "umicom/studio/platform_shell.h"
#include "umicom/studio/command_centre.h"
#include "umicom/studio/resource_explorer.h"
#include "umicom/studio/global_search.h"
#include "umicom/studio/extension_centre.h"
#include "umicom/studio/application_centre.h"
#include "umicom/studio/settings_centre.h"
#include "umicom/studio/welcome_centre.h"
#include "umicom/studio/developer_dashboard.h"
#include "umicom/studio/chart_workspace.h"
#include "umicom/studio/designer_workspace.h"
#include "umicom/studio/ai_workspace.h"
int main(void){UmiStudioPlatformShell*p=NULL;UmiStudioPlatformShellSnapshot a;UmiUiListModelSnapshot item={0};UmiStudioCommandCentreSnapshot b;UmiStudioResourceExplorerSnapshot c;UmiStudioGlobalSearchSnapshot d;UmiStudioExtensionCentreSnapshot e;UmiStudioApplicationCentreSnapshot f;UmiStudioSettingsCentreSnapshot g;UmiStudioWelcomeCentreSnapshot h;UmiStudioDeveloperDashboardSnapshot i;UmiStudioChartWorkspaceSnapshot j;UmiStudioDesignerWorkspaceSnapshot k;UmiStudioAiWorkspaceSnapshot l;if(umi_studio_platform_shell_create(NULL,&p)!=UMI_STATUS_OK)return 1;strcpy(item.id,"welcome");strcpy(item.label,"Welcome");item.visible=1;item.enabled=1;if(umi_ui_list_model_registry_upsert(umi_ui_workbench_platform_lists(umi_studio_platform_shell_workbench(p)),&item)!=UMI_STATUS_OK)return 2;if(umi_studio_platform_shell_snapshot(p,&a)!=UMI_STATUS_OK||a.workbench.list_items!=1U||!a.developer.available)return 3;if(umi_studio_command_centre_snapshot(NULL,&b)!=UMI_STATUS_OK)return 4;if(umi_studio_resource_explorer_snapshot(NULL,&c)!=UMI_STATUS_OK)return 5;if(umi_studio_global_search_snapshot(NULL,&d)!=UMI_STATUS_OK)return 6;if(umi_studio_extension_centre_snapshot(NULL,&e)!=UMI_STATUS_OK)return 7;if(umi_studio_application_centre_snapshot(NULL,&f)!=UMI_STATUS_OK)return 8;if(umi_studio_settings_centre_snapshot(NULL,&g)!=UMI_STATUS_OK)return 9;if(umi_studio_welcome_centre_snapshot(NULL,&h)!=UMI_STATUS_OK)return 10;if(umi_studio_developer_dashboard_snapshot(NULL,&i)!=UMI_STATUS_OK)return 11;if(umi_studio_chart_workspace_snapshot(NULL,&j)!=UMI_STATUS_OK)return 12;if(umi_studio_designer_workspace_snapshot(NULL,&k)!=UMI_STATUS_OK)return 13;if(umi_studio_ai_workspace_snapshot(NULL,&l)!=UMI_STATUS_OK)return 14;umi_studio_platform_shell_destroy(p);return 0;}
