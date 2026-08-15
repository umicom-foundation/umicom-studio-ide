/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_source_control_service.c
 *
 * PURPOSE:
 *   Verify Studio source-control service behaviour for a valid directory without a Git repository.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/source_control.h"

#include <assert.h>

int main(void)
{
    UmiStudioSourceControlService *service = NULL;
    UmiStudioSourceControlSnapshot snapshot;
    char temp_root[UMI_PATH_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    assert(umi_fs_temp_directory(temp_root, sizeof(temp_root)) == UMI_STATUS_OK);
    assert(umi_fs_join(root, sizeof(root), temp_root, "umicom-source-control-test") == UMI_STATUS_OK);
    (void)umi_fs_remove_tree(root);
    assert(umi_fs_make_directories(root) == UMI_STATUS_OK);
    assert(umi_studio_source_control_service_create(root, &service) == UMI_STATUS_OK);
    assert(umi_studio_source_control_service_snapshot(service, &snapshot) == UMI_STATUS_OK);
    assert(!snapshot.available);
    assert(umi_studio_source_control_service_refresh(service, 10U) == UMI_STATUS_UNAVAILABLE);
    assert(umi_studio_source_control_service_changes(service) != NULL);
    assert(umi_studio_source_control_service_history(service) != NULL);
    assert(umi_studio_source_control_service_workspace(service) != NULL);
    assert(umi_studio_source_control_service_coordinator(service) != NULL);
    assert(umi_studio_source_control_service_stage_all(service) == UMI_STATUS_UNAVAILABLE);
    assert(snapshot.revision >= 1U);
    umi_studio_source_control_service_destroy(service);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    return 0;
}
