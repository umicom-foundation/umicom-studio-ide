/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_editor_workbench_commands.c
 *
 * PURPOSE:
 *   Verify professional editor commands use the Framework document-view model
 *   and never discard dirty or pinned editors.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/bootstrap.h"
#include "umicom/studio/ui.h"
#include "umicom/studio/workbench_commands.h"

#include <assert.h>
#include <string.h>

static void add_document(UmiUiWorkbench *workbench,
                         const char *view_id,
                         int dirty)
{
    UmiUiDocumentViewSnapshot document = {0};
    (void)strcpy(document.view_id, view_id);
    (void)strcpy(document.document_id, view_id);
    (void)strcpy(document.title, view_id);
    (void)strcpy(document.group_id, "editor.primary");
    document.dirty = dirty;
    document.closable = 1;
    document.show_line_numbers = 1;
    assert(umi_ui_document_view_model_upsert(
               umi_ui_workbench_documents(workbench),
               &document) == UMI_STATUS_OK);
}

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiUiWorkbench *workbench;
    UmiUiWorkbenchSnapshot workbench_snapshot;
    UmiUiWorkbenchState state;
    UmiUiDocumentViewSnapshot document;
    char message[256];

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    workbench = umi_studio_ui_workbench(umi_studio_bootstrap_ui(bootstrap));

    add_document(workbench, "studio.editor.clean", 0);
    add_document(workbench, "studio.editor.unsaved", 1);
    assert(umi_command_registry_execute(
               umi_ui_workbench_commands(workbench),
               UMI_STUDIO_COMMAND_EDITOR_CLOSE_OTHERS,
               NULL,
               message,
               sizeof(message)) == UMI_STATUS_OK);
    assert(strstr(message, "Closed 1 editor") != NULL);
    assert(umi_ui_document_view_model_find(
               umi_ui_workbench_documents(workbench),
               "studio.editor.clean",
               &document) == UMI_STATUS_NOT_FOUND);
    assert(umi_ui_document_view_model_find(
               umi_ui_workbench_documents(workbench),
               "studio.editor.unsaved",
               &document) == UMI_STATUS_OK);

    assert(umi_command_registry_execute(
               umi_ui_workbench_commands(workbench),
               UMI_STUDIO_COMMAND_EDITOR_PIN_TOGGLE,
               NULL,
               NULL,
               0U) == UMI_STATUS_OK);
    assert(umi_ui_document_view_model_find(
               umi_ui_workbench_documents(workbench),
               "studio.editor.welcome",
               &document) == UMI_STATUS_OK);
    assert(!document.pinned);
    assert(umi_command_registry_execute(
               umi_ui_workbench_commands(workbench),
               UMI_STUDIO_COMMAND_EDITOR_PIN_TOGGLE,
               NULL,
               NULL,
               0U) == UMI_STATUS_OK);
    assert(umi_ui_document_view_model_find(
               umi_ui_workbench_documents(workbench),
               "studio.editor.welcome",
               &document) == UMI_STATUS_OK);
    assert(document.pinned);
    assert(umi_command_registry_execute(
               umi_ui_workbench_commands(workbench),
               UMI_STUDIO_COMMAND_EDITOR_PIN_TOGGLE,
               NULL,
               NULL,
               0U) == UMI_STATUS_OK);
    assert(umi_command_registry_execute(
               umi_ui_workbench_commands(workbench),
               UMI_STUDIO_COMMAND_EDITOR_NEXT,
               NULL,
               NULL,
               0U) == UMI_STATUS_OK);
    assert(umi_command_registry_execute(
               umi_ui_workbench_commands(workbench),
               UMI_STUDIO_COMMAND_EDITOR_PREVIOUS,
               NULL,
               NULL,
               0U) == UMI_STATUS_OK);
    assert(umi_command_registry_execute(
               umi_ui_workbench_commands(workbench),
               UMI_STUDIO_COMMAND_EDITOR_SPLIT_RIGHT,
               NULL,
               message,
               sizeof(message)) == UMI_STATUS_OK);
    assert(umi_ui_workbench_state_snapshot(workbench, &state) ==
           UMI_STATUS_OK);
    assert(state.editor_split_mode == UMI_UI_EDITOR_SPLIT_COLUMNS);
    assert(strcmp(state.active_editor_group,
                  UMI_UI_SECONDARY_EDITOR_GROUP_ID) == 0);
    assert(umi_ui_document_view_model_find(
               umi_ui_workbench_documents(workbench),
               "studio.editor.welcome", &document) == UMI_STATUS_OK);
    assert(strcmp(document.group_id,
                  UMI_UI_SECONDARY_EDITOR_GROUP_ID) == 0);

    assert(umi_command_registry_execute(
               umi_ui_workbench_commands(workbench),
               UMI_STUDIO_COMMAND_EDITOR_FOCUS_NEXT_GROUP,
               NULL, NULL, 0U) == UMI_STATUS_OK);
    assert(umi_ui_workbench_snapshot(workbench, &workbench_snapshot) ==
           UMI_STATUS_OK);
    assert(strcmp(workbench_snapshot.active_document_view,
                  "studio.editor.unsaved") == 0);

    assert(umi_command_registry_execute(
               umi_ui_workbench_commands(workbench),
               UMI_STUDIO_COMMAND_EDITOR_MOVE_NEXT_GROUP,
               NULL, NULL, 0U) == UMI_STATUS_OK);
    assert(umi_ui_document_view_model_find(
               umi_ui_workbench_documents(workbench),
               "studio.editor.unsaved", &document) == UMI_STATUS_OK);
    assert(strcmp(document.group_id,
                  UMI_UI_SECONDARY_EDITOR_GROUP_ID) == 0);
    assert(umi_command_registry_execute(
               umi_ui_workbench_commands(workbench),
               UMI_STUDIO_COMMAND_EDITOR_MOVE_PREVIOUS_GROUP,
               NULL, NULL, 0U) == UMI_STATUS_OK);
    assert(umi_ui_document_view_model_find(
               umi_ui_workbench_documents(workbench),
               "studio.editor.unsaved", &document) == UMI_STATUS_OK);
    assert(strcmp(document.group_id, UMI_UI_PRIMARY_EDITOR_GROUP_ID) == 0);

    state.editor_split_ratio = 7200;
    assert(umi_ui_workbench_state_apply(workbench, &state) == UMI_STATUS_OK);
    assert(umi_command_registry_execute(
               umi_ui_workbench_commands(workbench),
               UMI_STUDIO_COMMAND_EDITOR_BALANCE_GROUPS,
               NULL, NULL, 0U) == UMI_STATUS_OK);
    assert(umi_ui_workbench_state_snapshot(workbench, &state) ==
           UMI_STATUS_OK);
    assert(state.editor_split_ratio == UMI_UI_EDITOR_SPLIT_RATIO_DEFAULT);

    assert(umi_command_registry_execute(
               umi_ui_workbench_commands(workbench),
               UMI_STUDIO_COMMAND_EDITOR_SPLIT_DOWN,
               NULL, NULL, 0U) == UMI_STATUS_OK);
    assert(umi_ui_workbench_state_snapshot(workbench, &state) ==
           UMI_STATUS_OK);
    assert(state.editor_split_mode == UMI_UI_EDITOR_SPLIT_ROWS);
    assert(umi_command_registry_execute(
               umi_ui_workbench_commands(workbench),
               UMI_STUDIO_COMMAND_EDITOR_RESET_GROUPS,
               NULL, NULL, 0U) == UMI_STATUS_OK);
    assert(umi_ui_workbench_state_snapshot(workbench, &state) ==
           UMI_STATUS_OK);
    assert(state.editor_split_mode == UMI_UI_EDITOR_SPLIT_SINGLE);
    assert(umi_ui_document_view_model_group_count(
               umi_ui_workbench_documents(workbench),
               UMI_UI_SECONDARY_EDITOR_GROUP_ID) == 0U);
    assert(umi_command_registry_execute(
               umi_ui_workbench_commands(workbench),
               UMI_STUDIO_COMMAND_EDITOR_CLOSE_ALL,
               NULL,
               message,
               sizeof(message)) == UMI_STATUS_OK);
    assert(umi_ui_document_view_model_count(
               umi_ui_workbench_documents(workbench)) == 1U);
    assert(umi_ui_document_view_model_find(
               umi_ui_workbench_documents(workbench),
               "studio.editor.unsaved",
               &document) == UMI_STATUS_OK);
    assert(document.dirty);
    assert(umi_ui_workbench_snapshot(workbench,
                                     &workbench_snapshot) == UMI_STATUS_OK);
    assert(strcmp(workbench_snapshot.active_document_view,
                  "studio.editor.unsaved") == 0);

    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}
