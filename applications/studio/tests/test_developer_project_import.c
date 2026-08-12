/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_project_import.c
 *
 * PURPOSE:
 *   Verify Studio imports and activates a filesystem project through the
 *   shared Framework developer runtime without replacing existing centre tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include "umicom/studio/developer_workbench.h"

int main(void)
{
    UmiStudioDeveloperWorkbench *workbench = NULL;
    UmiDeveloperProjectBootstrapRequest request = {0};
    UmiDeveloperProjectBootstrapSnapshot snapshot;
    UmiStudioDeveloperSessionSnapshot session;
    UmiDeveloperContextSnapshot context;
    char temp_directory[UMI_PATH_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    char cmake_file[UMI_PATH_CAPACITY];
    char source_file[UMI_PATH_CAPACITY];

    if (umi_fs_temp_directory(temp_directory, sizeof(temp_directory)) != UMI_STATUS_OK)
        return 1;
    if (umi_path_join(temp_directory, "umicom-b31-studio-import",
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

    if (umi_studio_developer_workbench_create(&workbench) != UMI_STATUS_OK) return 8;
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEVELOPER_PROJECT_BOOTSTRAP_API_VERSION;
    request.project.struct_size = (uint32_t)sizeof(request.project);
    request.project.api_version = UMI_PROJECT_WORKSPACE_IMPORT_API_VERSION;
    request.project.root_directory = root;
    request.project.project_id = "studio-import";
    request.project.create_test_task = 1;
    request.preset = UMI_DEVELOPER_PROJECT_WORKFLOW_TEST;
    request.workflow_id = "studio.import.verify";
    request.prepare_workflow = 1;
    request.include_configure = 1;

    if (umi_studio_developer_workbench_import_project(
            workbench, &request, &snapshot) != UMI_STATUS_OK) return 9;
    if (!snapshot.workflow_prepared || !snapshot.project.validation.valid ||
        strcmp(snapshot.project.project_id, "studio-import") != 0) return 10;

    if (umi_developer_context_snapshot(
            umi_developer_runtime_context(
                umi_studio_developer_workbench_runtime(workbench)),
            &context) != UMI_STATUS_OK) return 11;
    if (strcmp(context.project_id, "studio-import") != 0) return 12;

    if (umi_studio_developer_session_snapshot(
            umi_studio_developer_workbench_session(workbench), &session) !=
        UMI_STATUS_OK) return 13;
    if (strcmp(session.project_id, "studio-import") != 0) return 14;

    umi_studio_developer_workbench_destroy(workbench);
    if (umi_fs_remove_tree(root) != UMI_STATUS_OK) return 15;
    return 0;
}
