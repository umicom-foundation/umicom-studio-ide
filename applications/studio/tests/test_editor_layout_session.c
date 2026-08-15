/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_editor_layout_session.c
 *
 * PURPOSE:
 *   Verify workspace-local editor-group ordering through Studio's existing
 *   Session Store integration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/editor_layout_session.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void add_document(UmiUiWorkbench *workbench,
                         const char *view_id,
                         const char *group_id)
{
    UmiUiDocumentViewSnapshot document = {0};
    (void)snprintf(document.view_id, sizeof(document.view_id), "%s", view_id);
    (void)snprintf(document.document_id, sizeof(document.document_id), "%s",
                   view_id);
    (void)snprintf(document.title, sizeof(document.title), "%s", view_id);
    (void)snprintf(document.group_id, sizeof(document.group_id), "%s",
                   group_id);
    document.closable = 1;
    assert(umi_ui_document_view_model_upsert(
               umi_ui_workbench_documents(workbench), &document) ==
           UMI_STATUS_OK);
}

int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiSessionStore *session = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiDocumentLayoutApplyResult result;
    UmiUiDocumentViewSnapshot document;

    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    assert(umi_session_store_create(&session) == UMI_STATUS_OK);
    assert(umi_ui_workbench_create("studio.editor-layout", commands,
                                   &workbench) == UMI_STATUS_OK);
    add_document(workbench, "view.one", UMI_UI_PRIMARY_EDITOR_GROUP_ID);
    add_document(workbench, "view.two", UMI_UI_PRIMARY_EDITOR_GROUP_ID);
    add_document(workbench, "view.three", UMI_UI_SECONDARY_EDITOR_GROUP_ID);
    assert(umi_ui_document_view_model_place(
               umi_ui_workbench_documents(workbench), "view.two",
               UMI_UI_PRIMARY_EDITOR_GROUP_ID, 0U) == UMI_STATUS_OK);
    assert(umi_ui_document_view_model_activate(
               umi_ui_workbench_documents(workbench), "view.three") ==
           UMI_STATUS_OK);
    assert(umi_studio_editor_layout_session_save(workbench, session) ==
           UMI_STATUS_OK);

    assert(umi_ui_document_view_model_place(
               umi_ui_workbench_documents(workbench), "view.two",
               UMI_UI_SECONDARY_EDITOR_GROUP_ID, 1U) == UMI_STATUS_OK);
    assert(umi_studio_editor_layout_session_restore(
               workbench, session, &result) == UMI_STATUS_OK);
    assert(result.applied_count == 3U && result.missing_count == 0U);
    assert(umi_ui_document_view_model_at(
               umi_ui_workbench_documents(workbench), 0U, &document) ==
           UMI_STATUS_OK);
    assert(strcmp(document.view_id, "view.two") == 0);
    assert(strcmp(document.group_id, UMI_UI_PRIMARY_EDITOR_GROUP_ID) == 0);
    assert(umi_ui_document_view_model_find(
               umi_ui_workbench_documents(workbench), "view.three",
               &document) == UMI_STATUS_OK);
    assert(document.active != 0);

    umi_ui_workbench_destroy(workbench);
    umi_session_store_destroy(session);
    umi_command_registry_destroy(commands);
    return EXIT_SUCCESS;
}
