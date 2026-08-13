/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_document_workbench.c
 *
 * PURPOSE:
 *   Verify Studio composes Framework document actions and commands without
 *   duplicating document I/O or editor behaviour in the application layer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioUi *ui;
    UmiUiActionSnapshot action;
    UmiDocumentWorkingCopySnapshot document;
    UmiCommandRegistry *commands;
    char message[512];

    (void)umi_fs_remove_tree(".umicom");
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    ui = umi_studio_bootstrap_ui(bootstrap);
    commands = umi_studio_bootstrap_command_registry(bootstrap);
    assert(ui != NULL && commands != NULL);
    assert(umi_document_coordinator_count(umi_studio_ui_documents(ui)) >= 1U);
    assert(umi_ui_action_model_find(
        umi_ui_workbench_actions(umi_studio_ui_workbench(ui)),
        "studio.action.file.open", &action) == UMI_STATUS_OK);
    assert(strcmp(action.command_id, UMI_DOCUMENT_COMMAND_OPEN) == 0);
    assert(action.argument_kind == UMI_UI_ACTION_ARGUMENT_OPEN_PATH);
    assert(umi_command_registry_execute(commands, UMI_DOCUMENT_COMMAND_NEW,
                                        "Workbench.c", message,
                                        sizeof(message)) == UMI_STATUS_OK);
    assert(umi_document_coordinator_active_snapshot(
        umi_studio_ui_documents(ui), &document) == UMI_STATUS_OK);
    assert(strcmp(document.display_name, "Workbench.c") == 0);
    assert(document.dirty && !document.has_path);
    assert(umi_studio_bootstrap_stop(bootstrap) == UMI_STATUS_OK);
    umi_studio_bootstrap_destroy(bootstrap);
    assert(umi_fs_remove_tree(".umicom") == UMI_STATUS_OK);
    (void)printf("Studio document workbench passed\n");
    return 0;
}
