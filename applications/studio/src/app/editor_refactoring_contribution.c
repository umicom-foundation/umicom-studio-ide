/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/editor_refactoring_contribution.c
 *
 * PURPOSE:
 *   Place Framework quick-fix, source-action and refactoring commands and
 *   views in Studio while all operational logic remains Framework-owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/editor_refactoring_contribution.h"

#include <string.h>

#define COMMAND(command, menu, group, position)                              \
    {                                                                         \
        (uint32_t)sizeof(UmiStudioEditorRefactoringCommandContribution),      \
        UMI_STUDIO_EDITOR_REFACTORING_CONTRIBUTION_API_VERSION,               \
        command, menu, group, position, 1                                     \
    }

static const UmiStudioEditorRefactoringCommandContribution COMMANDS[] = {
    COMMAND("editor.code-actions.show", "Edit", "code-actions", 900),
    COMMAND("editor.code-actions.apply-preferred", "Edit", "code-actions", 910),
    COMMAND("editor.code-actions.fix-all-document", "Edit", "code-actions", 920),
    COMMAND("editor.code-actions.organise-imports", "Edit", "code-actions", 930),
    COMMAND("editor.refactor.rename", "Refactor", "symbols", 100),
    COMMAND("editor.refactor.extract-function", "Refactor", "extract", 200),
    COMMAND("editor.refactor.extract-variable", "Refactor", "extract", 210),
    COMMAND("editor.refactor.extract-constant", "Refactor", "extract", 220),
    COMMAND("editor.refactor.inline", "Refactor", "transform", 300),
    COMMAND("editor.refactor.move", "Refactor", "transform", 310),
    COMMAND("editor.refactor.change-signature", "Refactor", "transform", 320),
    COMMAND("editor.refactor.preview.show", "Refactor", "preview", 400),
    COMMAND("editor.refactor.preview.include-edit", "Refactor", "preview", 410),
    COMMAND("editor.refactor.preview.exclude-edit", "Refactor", "preview", 420),
    COMMAND("editor.refactor.preview.include-all", "Refactor", "preview", 430),
    COMMAND("editor.refactor.preview.exclude-optional", "Refactor", "preview", 440),
    COMMAND("editor.refactor.apply", "Refactor", "transaction", 500),
    COMMAND("editor.refactor.cancel", "Refactor", "transaction", 510)
};

#undef COMMAND

#define VIEW(view, label, contract, region, position)                        \
    {                                                                         \
        (uint32_t)sizeof(UmiStudioEditorRefactoringViewContribution),         \
        UMI_STUDIO_EDITOR_REFACTORING_CONTRIBUTION_API_VERSION,               \
        view, label, contract, region, position, 1, 1                         \
    }

static const UmiStudioEditorRefactoringViewContribution VIEWS[] = {
    VIEW("studio.editor.code-actions", "Code Actions",
         "umicom.editor.code-action-query", "editor-overlay", 30),
    VIEW("studio.editor.refactoring-preview", "Refactoring Preview",
         "umicom.editor.refactoring-preview", "bottom-panel", 90),
    VIEW("studio.editor.edit-transaction", "Edit Transaction",
         "umicom.editor.edit-transaction", "bottom-panel", 100)
};

#undef VIEW

size_t umi_studio_editor_refactoring_command_contribution_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiStudioEditorRefactoringCommandContribution *
umi_studio_editor_refactoring_command_contribution_at(size_t index)
{
    return index < umi_studio_editor_refactoring_command_contribution_count()
        ? &COMMANDS[index] : NULL;
}

const UmiStudioEditorRefactoringCommandContribution *
umi_studio_editor_refactoring_command_contribution_find(
    const char *framework_command_id)
{
    size_t index;
    if (framework_command_id == NULL) return NULL;
    for (index = 0U;
         index < umi_studio_editor_refactoring_command_contribution_count();
         ++index) {
        if (strcmp(COMMANDS[index].framework_command_id,
                   framework_command_id) == 0) return &COMMANDS[index];
    }
    return NULL;
}

size_t umi_studio_editor_refactoring_view_contribution_count(void)
{
    return sizeof(VIEWS) / sizeof(VIEWS[0]);
}

const UmiStudioEditorRefactoringViewContribution *
umi_studio_editor_refactoring_view_contribution_at(size_t index)
{
    return index < umi_studio_editor_refactoring_view_contribution_count()
        ? &VIEWS[index] : NULL;
}

const UmiStudioEditorRefactoringViewContribution *
umi_studio_editor_refactoring_view_contribution_find(const char *view_id)
{
    size_t index;
    if (view_id == NULL) return NULL;
    for (index = 0U;
         index < umi_studio_editor_refactoring_view_contribution_count();
         ++index) {
        if (strcmp(VIEWS[index].view_id, view_id) == 0) return &VIEWS[index];
    }
    return NULL;
}
