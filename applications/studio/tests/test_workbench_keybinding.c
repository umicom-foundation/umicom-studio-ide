/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_workbench_keybinding.c
 *
 * PURPOSE:
 *   Verify a Studio keybinding resolves to one canonical command plus argument.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/ui.h"
#include "umicom/studio/workbench_commands.h"
#include "umicom/studio/workbench_shell_catalogue.h"
#include <assert.h>
#include <string.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiUiWorkbench *workbench;
    UmiUiKeybindingResolution resolution;
    UmiUiDocumentViewSnapshot document;

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    workbench = umi_studio_ui_workbench(umi_studio_bootstrap_ui(bootstrap));
    assert(umi_ui_workbench_resolve_keybinding(workbench,
                                               "Ctrl+Shift+F",
                                               &resolution) == UMI_STATUS_OK);
    assert(strcmp(resolution.command_id,
                  UMI_STUDIO_COMMAND_ACTIVITY_ACTIVATE) == 0);
    assert(strcmp(resolution.argument,
                  UMI_STUDIO_ACTIVITY_SEARCH) == 0);

    assert(umi_ui_workbench_resolve_keybinding(workbench,
                                               "Ctrl+Shift+P",
                                               &resolution) == UMI_STATUS_OK);
    assert(strcmp(resolution.command_id,
                  UMI_STUDIO_COMMAND_QUICK_ACCESS_SHOW) == 0);
    assert(umi_command_registry_execute(umi_ui_workbench_commands(workbench),
                                        resolution.command_id,
                                        NULL,
                                        NULL,
                                        0U) == UMI_STATUS_OK);
    {
        UmiUiContextSnapshot request;
        assert(umi_ui_context_get(
            umi_ui_workbench_context(workbench),
            UMI_UI_QUICK_ACCESS_REQUEST_CONTEXT_KEY,
            &request) == UMI_STATUS_OK);
        assert(request.kind == UMI_UI_CONTEXT_INTEGER);
        assert(request.integer_value == 1);
    }

    assert(umi_ui_workbench_resolve_keybinding(workbench,
                                               "Ctrl+Tab",
                                               &resolution) == UMI_STATUS_OK);
    assert(strcmp(resolution.command_id,
                  UMI_STUDIO_COMMAND_EDITOR_NEXT) == 0);
    assert(umi_ui_workbench_resolve_keybinding(workbench,
                                               "Ctrl+Shift+Tab",
                                               &resolution) == UMI_STATUS_OK);
    assert(strcmp(resolution.command_id,
                  UMI_STUDIO_COMMAND_EDITOR_PREVIOUS) == 0);
    assert(umi_ui_workbench_resolve_keybinding(workbench,
                                               "Ctrl+\\",
                                               &resolution) == UMI_STATUS_OK);
    assert(strcmp(resolution.command_id,
                  UMI_STUDIO_COMMAND_EDITOR_SPLIT_RIGHT) == 0);
    assert(umi_ui_workbench_resolve_keybinding(workbench,
                                               "Ctrl+Alt+Right",
                                               &resolution) == UMI_STATUS_OK);
    assert(strcmp(resolution.command_id,
                  UMI_STUDIO_COMMAND_EDITOR_MOVE_NEXT_GROUP) == 0);
    assert(umi_ui_workbench_resolve_keybinding(workbench,
                                               "Alt+Z",
                                               &resolution) == UMI_STATUS_OK);
    assert(strcmp(resolution.command_id,
                  UMI_STUDIO_COMMAND_EDITOR_WORD_WRAP_TOGGLE) == 0);
    assert(umi_command_registry_execute(umi_ui_workbench_commands(workbench),
                                        resolution.command_id,
                                        NULL,
                                        NULL,
                                        0U) == UMI_STATUS_OK);
    assert(umi_ui_document_view_model_find(
               umi_ui_workbench_documents(workbench),
               "studio.editor.welcome",
               &document) == UMI_STATUS_OK);
    assert(document.word_wrap);

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

    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}
