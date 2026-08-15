/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_source_control_workspace.c
 *
 * PURPOSE:
 *   Verify Studio composes its professional source-control workspace from the
 *   reusable Framework coordinator, including safe non-repository behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/studio/source_control.h"
#include "umicom/vcs_ui/vcs_ui.h"

int main(void)
{
    UmiStudioSourceControlService *service = NULL;
    UmiVcsWorkspaceCoordinator *coordinator;
    UmiVcsWorkspaceCoordinatorSnapshot snapshot;
    UmiUiViewModel *changes = NULL;
    UmiUiValue kind;
    UmiUiCommandViewAction action;
    char temporary[UMI_PATH_CAPACITY];
    char root[UMI_PATH_CAPACITY];

    assert(umi_fs_temp_directory(temporary, sizeof(temporary)) ==
           UMI_STATUS_OK);
    assert(umi_fs_join(root, sizeof(root), temporary,
                       "umicom-studio-source-control-workspace") ==
           UMI_STATUS_OK);
    (void)umi_fs_remove_tree(root);
    assert(umi_fs_make_directories(root) == UMI_STATUS_OK);
    assert(umi_studio_source_control_service_create(root, &service) ==
           UMI_STATUS_OK);
    coordinator = umi_studio_source_control_service_coordinator(service);
    assert(coordinator != NULL);
    assert(umi_vcs_workspace_coordinator_snapshot(coordinator, &snapshot) ==
           UMI_STATUS_OK);
    assert(!snapshot.repository.available);
    assert(strcmp(snapshot.change_filter_label, "all changes") == 0);
    assert(umi_vcs_workspace_coordinator_set_change_filter(
               coordinator, UMI_VCS_CHANGE_FILTER_CONFLICTS) ==
           UMI_STATUS_OK);
    assert(umi_vcs_workspace_coordinator_set_commit_message(
               coordinator, "feat: source control workspace") ==
           UMI_STATUS_OK);
    assert(umi_vcs_ui_workspace_changes_view_create(
               "studio.source-control-test", coordinator, &changes) ==
           UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
               changes, "umicom.view-kind", &kind) == UMI_STATUS_OK);
    assert(kind.kind == UMI_UI_VALUE_STRING);
    assert(strcmp(kind.string_value, "source-control-changes") == 0);
    assert(umi_ui_command_view_action_at(changes, 0U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id, "studio.action.vcs.refresh") == 0);
    assert(!action.enabled);

    umi_ui_view_model_destroy(changes);
    umi_studio_source_control_service_destroy(service);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    return 0;
}
