/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_project_centre.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include "umicom/studio/project_centre.h"

static int populate_project(UmiProjectWorkspace *workspace)
{
    UmiProjectDescriptorSnapshot project = {0};
    UmiProjectConfigurationSnapshot configuration = {0};
    UmiProjectTargetSnapshot target = {0};
    UmiProjectTaskSnapshot task = {0};
    UmiProjectLaunchProfileSnapshot launch = {0};
    UmiProjectEnvironmentSnapshot environment = {0};

    project.struct_size=(uint32_t)sizeof(project);
    project.api_version=UMI_PROJECT_DESCRIPTOR_API_VERSION;
    strcpy(project.id,"studio"); strcpy(project.name,"Umicom Studio");
    strcpy(project.root_uri,"."); project.enabled=1;
    if(umi_project_descriptor_registry_upsert(
        umi_project_workspace_descriptor(workspace),&project)!=UMI_STATUS_OK)return 1;

    configuration.struct_size=(uint32_t)sizeof(configuration);
    configuration.api_version=UMI_PROJECT_CONFIGURATION_API_VERSION;
    strcpy(configuration.id,"debug"); strcpy(configuration.project_id,"studio");
    strcpy(configuration.name,"Debug"); strcpy(configuration.toolchain_id,"clang");
    configuration.active=1;
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

    environment.struct_size=(uint32_t)sizeof(environment);
    environment.api_version=UMI_PROJECT_ENVIRONMENT_API_VERSION;
    strcpy(environment.id,"local"); strcpy(environment.project_id,"studio");
    strcpy(environment.name,"Local"); strcpy(environment.toolchain_id,"clang");
    environment.inherit_parent=1;
    if(umi_project_environment_registry_upsert(
        umi_project_workspace_environment(workspace),&environment)!=UMI_STATUS_OK)return 6;
    return 0;
}

static int test_filesystem_import(void)
{
    UmiStudioProjectCentre *centre = NULL;
    UmiProjectWorkspaceImportRequest request = {0};
    UmiProjectWorkspaceImportSnapshot imported;
    UmiStudioProjectCentreSnapshot snapshot;
    char temp_directory[UMI_PATH_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    char cmake_file[UMI_PATH_CAPACITY];
    char source_file[UMI_PATH_CAPACITY];

    if (umi_fs_temp_directory(temp_directory, sizeof(temp_directory)) != UMI_STATUS_OK)
        return 1;
    if (umi_path_join(temp_directory, "umicom-b31-studio-project-centre",
                      root, sizeof(root)) != UMI_STATUS_OK) return 2;
    (void)umi_fs_remove_tree(root);
    if (umi_fs_make_directories(root) != UMI_STATUS_OK) return 3;
    if (umi_path_join(root, "CMakeLists.txt", cmake_file,
                      sizeof(cmake_file)) != UMI_STATUS_OK) return 4;
    if (umi_path_join(root, "main.c", source_file,
                      sizeof(source_file)) != UMI_STATUS_OK) return 5;
    if (umi_fs_write_text(cmake_file,
        "cmake_minimum_required(VERSION 3.24)\nproject(studio_import C)\n") !=
        UMI_STATUS_OK) return 6;
    if (umi_fs_write_text(source_file,
        "int main(void) { return 0; }\n") != UMI_STATUS_OK) return 7;

    if (umi_studio_project_centre_create(&centre) != UMI_STATUS_OK) return 8;
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_PROJECT_WORKSPACE_IMPORT_API_VERSION;
    request.root_directory = root;
    request.project_id = "studio-import";
    request.create_test_task = 1;
    request.launch_program = "studio-import-app";
    if (umi_studio_project_centre_import_directory(
            centre, &request, &imported) != UMI_STATUS_OK) return 9;
    if (!imported.has_cmake || !imported.validation.valid ||
        strcmp(imported.selection.project.id, "studio-import") != 0)
        return 10;
    if (umi_studio_project_centre_snapshot(centre, &snapshot) != UMI_STATUS_OK ||
        !snapshot.has_selection || snapshot.service.task_count != 3U ||
        snapshot.service.launch_profile_count != 1U ||
        !snapshot.has_workspace_model ||
        snapshot.workspace_model.root_count != 1U ||
        snapshot.workspace_model.member_count != 1U ||
        strcmp(snapshot.workspace_model.active_project_id,
               "studio-import") != 0) return 11;

    umi_studio_project_centre_destroy(centre);
    if (umi_fs_remove_tree(root) != UMI_STATUS_OK) return 12;
    return 0;
}

int main(void)
{
    UmiStudioProjectCentre *centre=NULL;
    UmiStudioProjectCentreSnapshot snapshot;
    UmiProjectWorkspaceSelectionSnapshot selection;
    UmiProjectWorkspaceValidationReport validation;
    int result;

    if(umi_studio_project_centre_create(&centre)!=UMI_STATUS_OK)return 1;
    result=populate_project(umi_studio_project_centre_service(centre));
    if(result!=0)return 10+result;

    if(umi_studio_project_centre_select_project(
        centre,"studio",&selection)!=UMI_STATUS_OK)return 20;
    if(strcmp(selection.project.id,"studio")!=0||
       !selection.has_configuration||!selection.has_target||
       !selection.has_task||!selection.has_launch_profile||
       !selection.has_environment)return 21;

    if(umi_studio_project_centre_validate(centre,&validation)!=UMI_STATUS_OK||
       validation.valid==0)return 22;
    if(umi_studio_project_centre_snapshot(centre,&snapshot)!=UMI_STATUS_OK||
       !snapshot.available||!snapshot.has_selection||
       strcmp(snapshot.selection.project.id,"studio")!=0||
       snapshot.validation.valid==0)return 23;

    umi_studio_project_centre_destroy(centre);

    result = test_filesystem_import();
    if (result != 0) return 100 + result;
    return 0;
}
