/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_workspace_discovery.c
 *
 * PURPOSE:
 *   Verify Studio's Project Centre exposes non-destructive CMake discovery and
 *   preserves the last refresh plan in its product-facing snapshot.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include "umicom/studio/project_centre.h"

int main(void)
{
    UmiStudioProjectCentre *centre = NULL;
    UmiProjectWorkspaceRootSnapshot root_snapshot = {0};
    UmiProjectWorkspaceExclusionSnapshot exclusion = {0};
    UmiProjectWorkspaceRefreshSnapshot refresh;
    UmiStudioProjectCentreSnapshot snapshot;
    char temporary[UMI_PATH_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    char ignored[UMI_PATH_CAPACITY];
    char cmake_file[UMI_PATH_CAPACITY];

    if (umi_fs_temp_directory(temporary, sizeof(temporary)) != UMI_STATUS_OK)
        return 1;
    if (umi_path_join(temporary, "umicom-b34-studio-discovery",
                      root, sizeof(root)) != UMI_STATUS_OK) return 2;
    (void)umi_fs_remove_tree(root);
    if (umi_fs_make_directories(root) != UMI_STATUS_OK) return 3;
    if (umi_path_join(root, "CMakeLists.txt", cmake_file,
                      sizeof(cmake_file)) != UMI_STATUS_OK ||
        umi_fs_write_text(cmake_file, "project(studio_fixture C)\n") !=
            UMI_STATUS_OK) return 4;
    if (umi_path_join(root, "vendor", ignored, sizeof(ignored)) !=
            UMI_STATUS_OK ||
        umi_fs_make_directories(ignored) != UMI_STATUS_OK ||
        umi_path_join(ignored, "CMakeLists.txt", cmake_file,
                      sizeof(cmake_file)) != UMI_STATUS_OK ||
        umi_fs_write_text(cmake_file, "project(vendor C)\n") != UMI_STATUS_OK)
        return 5;

    if (umi_studio_project_centre_create(&centre) != UMI_STATUS_OK) return 6;
    root_snapshot.struct_size = (uint32_t)sizeof(root_snapshot);
    root_snapshot.api_version = UMI_PROJECT_WORKSPACE_MODEL_API_VERSION;
    strcpy(root_snapshot.id, "fixture");
    strcpy(root_snapshot.path, root);
    root_snapshot.enabled = 1;
    if (umi_studio_project_centre_upsert_workspace_root(
            centre, &root_snapshot) != UMI_STATUS_OK) return 7;
    exclusion.struct_size = (uint32_t)sizeof(exclusion);
    exclusion.api_version = UMI_PROJECT_WORKSPACE_EXCLUSION_API_VERSION;
    strcpy(exclusion.id, "vendor");
    strcpy(exclusion.pattern, "vendor");
    exclusion.kind = UMI_PROJECT_WORKSPACE_EXCLUDE_PATH_SEGMENT;
    exclusion.enabled = 1;
    if (umi_studio_project_centre_upsert_workspace_exclusion(
            centre, &exclusion) != UMI_STATUS_OK) return 8;

    if (umi_studio_project_centre_plan_workspace_refresh(
            centre, NULL, &refresh) != UMI_STATUS_OK) return 9;
    if (refresh.discovery.project_count != 1U ||
        refresh.import_candidate_count != 1U ||
        !refresh.requires_confirmation ||
        refresh.discovery.excluded_directory_count == 0U) return 10;
    if (umi_studio_project_centre_snapshot(centre, &snapshot) != UMI_STATUS_OK ||
        !snapshot.has_refresh_plan ||
        snapshot.refresh.import_candidate_count != 1U ||
        snapshot.refresh.discovered_project_count != 1U) return 11;

    umi_studio_project_centre_destroy(centre);
    if (umi_fs_remove_tree(root) != UMI_STATUS_OK) return 12;
    return 0;
}
