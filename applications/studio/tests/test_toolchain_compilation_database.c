/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_toolchain_compilation_database.c
 * PURPOSE: Verify centre-level compile_commands.json import and reporting.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stdlib.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include "umicom/studio/toolchain_centre.h"

int main(void)
{
    UmiStudioToolchainCentre *centre = NULL;
    UmiStudioToolchainCentreSnapshot snapshot;
    char temporary[UMI_TOOL_PATH_CAPACITY];
    char path[UMI_TOOL_PATH_CAPACITY];
    char build_directory[UMI_TOOL_PATH_CAPACITY];
    char discovered_path[UMI_TOOL_PATH_CAPACITY];

    if (umi_fs_temp_directory(temporary, sizeof(temporary)) != UMI_STATUS_OK ||
        umi_path_join(temporary, "build", build_directory,
                      sizeof(build_directory)) != UMI_STATUS_OK ||
        umi_fs_make_directories(build_directory) != UMI_STATUS_OK ||
        umi_path_join(build_directory, "compile_commands.json", path,
                      sizeof(path)) != UMI_STATUS_OK ||
        umi_fs_write_text(path,
            "[{\"directory\":\"/src/build\","
            "\"arguments\":[\"clang\",\"-c\",\"/src/main.c\"],"
            "\"file\":\"/src/main.c\"}]") != UMI_STATUS_OK ||
        umi_studio_toolchain_centre_create(&centre) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    if (umi_studio_toolchain_centre_discover_compilation_database(
            centre, temporary, NULL, 0U, discovered_path,
            sizeof(discovered_path)) !=
            UMI_STATUS_OK ||
        umi_studio_toolchain_centre_snapshot(centre, &snapshot) !=
            UMI_STATUS_OK ||
        !snapshot.has_compilation_database ||
        snapshot.compilation_database.command_count != 1U ||
        snapshot.compilation_database.clang_command_count != 1U) {
        umi_studio_toolchain_centre_destroy(centre);
        (void)umi_fs_remove_tree(build_directory);
        return EXIT_FAILURE;
    }
    umi_studio_toolchain_centre_destroy(centre);
    if (umi_fs_remove_tree(build_directory) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
