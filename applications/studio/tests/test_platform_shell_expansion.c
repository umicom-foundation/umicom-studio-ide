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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
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

static int populate_project(UmiStudioPlatformShell *shell)
{
    UmiStudioDeveloperWorkbench *developer =
        umi_studio_platform_shell_developer(shell);
    UmiProjectWorkspace *workspace =
        umi_studio_project_centre_service(
            umi_studio_developer_workbench_projects(developer));
    UmiProjectDescriptorSnapshot project={0};
    UmiProjectConfigurationSnapshot configuration={0};
    UmiProjectTargetSnapshot target={0};
    UmiProjectTaskSnapshot task={0};

    project.struct_size=(uint32_t)sizeof(project);
    project.api_version=UMI_PROJECT_DESCRIPTOR_API_VERSION;
    strcpy(project.id,"studio"); strcpy(project.name,"Umicom Studio");
    strcpy(project.root_uri,"."); project.enabled=1;
    if(umi_project_descriptor_registry_upsert(
        umi_project_workspace_descriptor(workspace),&project)!=UMI_STATUS_OK)return 1;

    configuration.struct_size=(uint32_t)sizeof(configuration);
    configuration.api_version=UMI_PROJECT_CONFIGURATION_API_VERSION;
    strcpy(configuration.id,"debug"); strcpy(configuration.project_id,"studio");
    strcpy(configuration.name,"Debug"); configuration.active=1;
    if(umi_project_configuration_registry_upsert(
        umi_project_workspace_configuration(workspace),&configuration)!=UMI_STATUS_OK)return 2;

    target.struct_size=(uint32_t)sizeof(target);
    target.api_version=UMI_PROJECT_TARGET_API_VERSION;
    strcpy(target.id,"studio-console"); strcpy(target.project_id,"studio");
    strcpy(target.name,"Studio Console"); target.enabled=1; target.default_target=1;
    if(umi_project_target_registry_upsert(
        umi_project_workspace_target(workspace),&target)!=UMI_STATUS_OK)return 3;

    task.struct_size=(uint32_t)sizeof(task);
    task.api_version=UMI_PROJECT_TASK_API_VERSION;
    strcpy(task.id,"build"); strcpy(task.project_id,"studio");
    strcpy(task.label,"Build"); strcpy(task.command,"cmake --build build");
    strcpy(task.group,"build"); task.enabled=1; task.default_task=1;
    if(umi_project_task_registry_upsert(
        umi_project_workspace_task(workspace),&task)!=UMI_STATUS_OK)return 4;
    return 0;
}

static int import_project(UmiStudioPlatformShell *shell, char *root, size_t capacity)
{
    UmiDeveloperProjectBootstrapRequest request={0};
    UmiDeveloperProjectBootstrapSnapshot *bootstrap=NULL;
    char temp_directory[UMI_PATH_CAPACITY];
    char cmake_file[UMI_PATH_CAPACITY];
    char source_file[UMI_PATH_CAPACITY];

    if(umi_fs_temp_directory(temp_directory,sizeof(temp_directory))!=UMI_STATUS_OK)return 1;
    if(umi_path_join(temp_directory,"umicom-b31-platform-shell",root,capacity)!=UMI_STATUS_OK)return 2;
    (void)umi_fs_remove_tree(root);
    if(umi_fs_make_directories(root)!=UMI_STATUS_OK)return 3;
    if(umi_path_join(root,"CMakeLists.txt",cmake_file,sizeof(cmake_file))!=UMI_STATUS_OK)return 4;
    if(umi_path_join(root,"main.c",source_file,sizeof(source_file))!=UMI_STATUS_OK)return 5;
    if(umi_fs_write_text(cmake_file,
        "cmake_minimum_required(VERSION 3.24)\nproject(shell_import C)\n")!=UMI_STATUS_OK)return 6;
    if(umi_fs_write_text(source_file,"int main(void) { return 0; }\n")!=UMI_STATUS_OK)return 7;

    request.struct_size=(uint32_t)sizeof(request);
    request.api_version=UMI_DEVELOPER_PROJECT_BOOTSTRAP_API_VERSION;
    request.project.struct_size=(uint32_t)sizeof(request.project);
    request.project.api_version=UMI_PROJECT_WORKSPACE_IMPORT_API_VERSION;
    request.project.root_directory=root;
    request.project.project_id="shell-import";
    request.project.create_test_task=1;
    request.preset=UMI_DEVELOPER_PROJECT_WORKFLOW_TEST;
    request.workflow_id="shell.import.test";
    request.prepare_workflow=1;
    request.include_configure=1;
    bootstrap=(UmiDeveloperProjectBootstrapSnapshot *)calloc(1U,sizeof(*bootstrap));
    if(bootstrap==NULL)return 8;
    if(umi_studio_platform_shell_import_project(
        shell,&request,bootstrap)!=UMI_STATUS_OK){free(bootstrap);return 8;}
    if(!bootstrap->workflow_prepared||bootstrap->workflow.workflow.operation_count!=3U||
       strcmp(bootstrap->context.project_id,"shell-import")!=0){free(bootstrap);return 9;}
    free(bootstrap);
    return 0;
}

int main(void)
{
    UmiStudioServices *services=NULL;
    UmiStudioPlatformShell *p=NULL;
    UmiStudioPlatformShellSnapshot *a=NULL;
    UmiUiListModelSnapshot item={0};
    UmiStudioCommandCentreSnapshot b;
    UmiStudioResourceExplorerSnapshot c;
    UmiStudioGlobalSearchSnapshot d;
    UmiStudioExtensionCentreSnapshot e;
    UmiStudioApplicationCentreSnapshot f;
    UmiStudioSettingsCentreSnapshot g;
    UmiStudioWelcomeCentreSnapshot h;
    UmiStudioDeveloperDashboardSnapshot i;
    UmiStudioChartWorkspaceSnapshot j;
    UmiStudioDesignerWorkspaceSnapshot k;
    UmiStudioAiWorkspaceSnapshot l;
    UmiProjectWorkspaceSelectionRequest selection_request={0};
    UmiProjectWorkspaceSelectionSnapshot selection;
    UmiProjectWorkspaceValidationReport validation;
    UmiDeveloperProjectWorkflowRequest workflow_request={0};
    UmiDeveloperProjectWorkflowSnapshot workflow;
    char imported_root[UMI_PATH_CAPACITY];
    int result;

    if(umi_studio_services_create(NULL,NULL,&services)!=UMI_STATUS_OK)return 1;
    if(umi_studio_platform_shell_create(services,&p)!=UMI_STATUS_OK){
        umi_studio_services_destroy(services);
        return 1;
    }
    strcpy(item.id,"welcome");strcpy(item.label,"Welcome");item.visible=1;item.enabled=1;
    if(umi_ui_list_model_registry_upsert(
        umi_ui_workbench_platform_lists(umi_studio_platform_shell_workbench(p)),
        &item)!=UMI_STATUS_OK)return 2;

    result=populate_project(p);
    if(result!=0)return 20+result;

    selection_request.struct_size=(uint32_t)sizeof(selection_request);
    selection_request.api_version=UMI_PROJECT_WORKSPACE_QUERY_API_VERSION;
    selection_request.project_id="studio";
    if(umi_studio_platform_shell_activate_project(
        p,&selection_request,&selection)!=UMI_STATUS_OK)return 30;
    if(strcmp(selection.project.id,"studio")!=0)return 31;
    if(umi_studio_platform_shell_validate_project(
        p,&validation)!=UMI_STATUS_OK||validation.valid==0)return 32;

    workflow_request.struct_size=(uint32_t)sizeof(workflow_request);
    workflow_request.api_version=UMI_DEVELOPER_PROJECT_WORKFLOW_API_VERSION;
    workflow_request.preset=UMI_DEVELOPER_PROJECT_WORKFLOW_BUILD;
    workflow_request.project_id="studio";
    if(umi_studio_platform_shell_prepare_project_workflow(
        p,&workflow_request,&workflow)!=UMI_STATUS_OK)return 33;
    if(workflow.workflow.operation_count!=1U)return 34;

    result=import_project(p,imported_root,sizeof(imported_root));
    if(result!=0)return 40+result;

    a=(UmiStudioPlatformShellSnapshot *)calloc(1U,sizeof(*a));
    if(a==NULL)return 50;
    if(umi_studio_platform_shell_snapshot(p,a)!=UMI_STATUS_OK||
       a->workbench.list_items!=1U||!a->developer.available||
       !a->developer.projects.has_selection||
       strcmp(a->developer.projects.selection.project.id,"shell-import")!=0||
       !a->developer.pipeline.has_project_workflow){free(a);return 50;}
    free(a);

    /*
     * Existing platform-centre smoke coverage remains intact.
     */
    if(umi_studio_command_centre_snapshot(NULL,&b)!=UMI_STATUS_OK)return 4;
    if(umi_studio_resource_explorer_snapshot(NULL,&c)!=UMI_STATUS_OK)return 5;
    if(umi_studio_global_search_snapshot(NULL,&d)!=UMI_STATUS_OK)return 6;
    if(umi_studio_extension_centre_snapshot(NULL,&e)!=UMI_STATUS_OK)return 7;
    if(umi_studio_application_centre_snapshot(NULL,&f)!=UMI_STATUS_OK)return 8;
    if(umi_studio_settings_centre_snapshot(NULL,&g)!=UMI_STATUS_OK)return 9;
    if(umi_studio_welcome_centre_snapshot(NULL,&h)!=UMI_STATUS_OK)return 10;
    if(umi_studio_developer_dashboard_snapshot(NULL,&i)!=UMI_STATUS_OK)return 11;
    if(umi_studio_chart_workspace_snapshot(NULL,&j)!=UMI_STATUS_OK)return 12;
    if(umi_studio_designer_workspace_snapshot(NULL,&k)!=UMI_STATUS_OK)return 13;
    if(umi_studio_ai_workspace_snapshot(services,&l)!=UMI_STATUS_OK)return 14;

    umi_studio_platform_shell_destroy(p);
    umi_studio_services_destroy(services);
    if(umi_fs_remove_tree(imported_root)!=UMI_STATUS_OK)return 51;
    return 0;
}
