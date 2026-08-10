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
    const char *root = ".umicom-source-control-test";
    (void)umi_fs_remove_tree(root);
    assert(umi_fs_make_directories(root) == UMI_STATUS_OK);
    assert(umi_studio_source_control_service_create(root, &service) == UMI_STATUS_OK);
    assert(umi_studio_source_control_service_snapshot(service, &snapshot) == UMI_STATUS_OK);
    assert(!snapshot.available);
    assert(umi_studio_source_control_service_refresh(service, 10U) == UMI_STATUS_UNAVAILABLE);
    assert(umi_studio_source_control_service_changes(service) != NULL);
    assert(umi_studio_source_control_service_history(service) != NULL);
    umi_studio_source_control_service_destroy(service);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    return 0;
}
