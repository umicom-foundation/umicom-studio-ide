/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_watcher.c
 *
 * PURPOSE:
 *   Verify that Studio watcher scans update operational statistics and the
 *   shared workspace file index.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/watcher.h"
#include "umicom/studio/workspace.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    UmiStudioServices *services = NULL;
    UmiWatcherStats watcher_stats;
    UmiFileIndexStats index_stats;
    char temporary[UMI_PATH_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    char file[UMI_PATH_CAPACITY];

    assert(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
    assert(umi_path_join(temporary,
                         "umicom-studio-watcher-test",
                         root,
                         sizeof(root)) == UMI_STATUS_OK);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    assert(umi_fs_make_directories(root) == UMI_STATUS_OK);
    assert(umi_studio_services_create(NULL, NULL, &services) == UMI_STATUS_OK);
    assert(umi_studio_workspace_open(services, root, 1, 0) == UMI_STATUS_OK);
    assert(umi_studio_watcher_scan_once(services) == UMI_STATUS_OK);
    assert(umi_path_join(root, "watched.c", file, sizeof(file)) == UMI_STATUS_OK);
    assert(umi_fs_write_text(file, "int value;\n") == UMI_STATUS_OK);
    assert(umi_studio_watcher_scan_once(services) == UMI_STATUS_OK);

    watcher_stats = umi_studio_watcher_stats(services);
    index_stats = umi_file_index_stats(umi_studio_services_file_index(services));
    assert(watcher_stats.scans == 2U);
    assert(watcher_stats.created >= 1U);
    assert(index_stats.files == 1U);

    umi_studio_services_destroy(services);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    return 0;
}
