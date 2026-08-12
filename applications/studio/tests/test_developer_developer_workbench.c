/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_developer_workbench.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "umicom/studio/developer_workbench.h"

static int populate(UmiStudioDeveloperWorkbench *workbench)
{
    UmiProjectWorkspace *workspace =
        umi_studio_project_centre_service(
            umi_studio_developer_workbench_projects(workbench));
    UmiProjectDescriptorSnapshot project={0};
    UmiProjectConfigurationSnapshot configuration={0};
    UmiProjectTargetSnapshot target={0};
    UmiProjectTaskSnapshot task={0};
    UmiProjectLaunchProfileSnapshot launch={0};

    project.struct_size=(uint32_t)sizeof(project);
    project.api_version=UMI_PROJECT_DESCRIPTOR_API_VERSION;
    strcpy(project.id,"studio"); strcpy(project.name,"Umicom Studio");
    strcpy(project.root_uri,"C:/Dev/umicom/umicom-studio"); project.enabled=1;
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

    launch.struct_size=(uint32_t)sizeof(launch);
    launch.api_version=UMI_PROJECT_LAUNCH_PROFILE_API_VERSION;
    strcpy(launch.id,"run"); strcpy(launch.project_id,"studio");
    strcpy(launch.name,"Run"); strcpy(launch.program,"umicom-studio-console");
    launch.default_profile=1;
    if(umi_project_launch_profile_registry_upsert(
        umi_project_workspace_launch_profile(workspace),&launch)!=UMI_STATUS_OK)return 5;
    return 0;
}

int main(void)
{
    UmiStudioDeveloperWorkbench *workbench=NULL;
    UmiStudioDeveloperWorkbenchSnapshot snapshot;
    UmiProjectWorkspaceSelectionRequest request={0};
    UmiProjectWorkspaceSelectionSnapshot selection;
    UmiProjectWorkspaceValidationReport validation;
    UmiDeveloperContextSnapshot context;
    UmiStudioDeveloperSessionSnapshot session;
    UmiDeveloperProjectWorkflowRequest workflow_request={0};
    UmiDeveloperProjectWorkflowSnapshot workflow;
    int result;

    if(umi_studio_developer_workbench_create(&workbench)!=UMI_STATUS_OK)return 1;
    result=populate(workbench);
    if(result!=0)return 10+result;

    request.struct_size=(uint32_t)sizeof(request);
    request.api_version=UMI_PROJECT_WORKSPACE_QUERY_API_VERSION;
    request.project_id="studio";
    if(umi_studio_developer_workbench_activate_project(
        workbench,&request,&selection)!=UMI_STATUS_OK)return 20;
    if(strcmp(selection.project.id,"studio")!=0)return 21;

    if(umi_developer_context_snapshot(
        umi_developer_runtime_context(
            umi_studio_developer_workbench_runtime(workbench)),
        &context)!=UMI_STATUS_OK)return 22;
    if(strcmp(context.project_id,"studio")!=0||
       strcmp(context.configuration_id,"debug")!=0||
       strcmp(context.target_id,"studio-console")!=0||
       strcmp(context.launch_profile_id,"run")!=0)return 23;

    if(umi_studio_developer_session_snapshot(
        umi_studio_developer_workbench_session(workbench),
        &session)!=UMI_STATUS_OK)return 24;
    if(strcmp(session.project_id,"studio")!=0||
       strcmp(session.configuration_id,"debug")!=0||
       strcmp(session.launch_profile_id,"run")!=0)return 25;

    if(umi_studio_developer_workbench_validate_project(
        workbench,&validation)!=UMI_STATUS_OK||validation.valid==0)return 26;

    workflow_request.struct_size=(uint32_t)sizeof(workflow_request);
    workflow_request.api_version=UMI_DEVELOPER_PROJECT_WORKFLOW_API_VERSION;
    workflow_request.preset=UMI_DEVELOPER_PROJECT_WORKFLOW_BUILD;
    workflow_request.project_id="studio";
    if(umi_studio_developer_workbench_prepare_project_workflow(
        workbench,&workflow_request,&workflow)!=UMI_STATUS_OK)return 27;
    if(workflow.workflow.operation_count!=1U)return 28;

    if(umi_studio_developer_workbench_snapshot(
        workbench,&snapshot)!=UMI_STATUS_OK||!snapshot.available||
       !snapshot.projects.has_selection||
       !snapshot.pipeline.has_project_workflow)return 29;

    umi_studio_developer_workbench_destroy(workbench);
    return 0;
}
