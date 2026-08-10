/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_files.c
 *
 * PURPOSE:
 *   Verify trusted workspace file creation, reading, appending, renaming,
 *   deletion, index maintenance and path-escape protection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/files.h"
#include "umicom/studio/workspace.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiStudioServices *services = NULL;
    UmiFileIndexStats stats;
    char temporary[UMI_PATH_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    char *text = NULL;
    size_t size = 0U;

    assert(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
    assert(umi_path_join(temporary,
                         "umicom-studio-files-test",
                         root,
                         sizeof(root)) == UMI_STATUS_OK);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    assert(umi_fs_make_directories(root) == UMI_STATUS_OK);
    assert(umi_studio_services_create(NULL, NULL, &services) == UMI_STATUS_OK);
    assert(umi_studio_workspace_open(services, root, 1, 0) == UMI_STATUS_OK);

    assert(umi_studio_files_create_directory(services, "src") == UMI_STATUS_OK);
    assert(umi_studio_files_write_text(services,
                                       "src/main.c",
                                       "alpha") == UMI_STATUS_OK);
    assert(umi_studio_files_append_text(services,
                                        "src/main.c",
                                        " beta") == UMI_STATUS_OK);
    assert(umi_studio_files_read_text(services,
                                      "src/main.c",
                                      &text,
                                      &size) == UMI_STATUS_OK);
    assert(size == 10U);
    assert(strcmp(text, "alpha beta") == 0);
    umi_fs_free_text(text);
    assert(umi_studio_files_rename(services,
                                   "src/main.c",
                                   "src/app.c") == UMI_STATUS_OK);
    stats = umi_file_index_stats(umi_studio_services_file_index(services));
    assert(stats.files == 1U);
    assert(umi_studio_files_remove(services, "src/app.c") == UMI_STATUS_OK);
    stats = umi_file_index_stats(umi_studio_services_file_index(services));
    assert(stats.files == 0U);
    assert(umi_studio_files_write_text(services,
                                       "../escape.txt",
                                       "blocked") == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_studio_workspace_set_trusted(services, 0) == UMI_STATUS_OK);
    assert(umi_studio_files_write_text(services,
                                       "blocked.txt",
                                       "blocked") == UMI_STATUS_PERMISSION_DENIED);

    umi_studio_services_destroy(services);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    return 0;
}
