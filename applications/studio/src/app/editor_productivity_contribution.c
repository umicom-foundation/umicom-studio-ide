/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/editor_productivity_contribution.c
 *
 * PURPOSE:
 *   Place Framework editing-productivity commands and views in Studio while
 *   all parsing, validation, session and transformation logic stays Framework-owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/editor_productivity_contribution.h"

#include <string.h>

#define COMMAND(command, menu, group, position)                                \
    {                                                                           \
        (uint32_t)sizeof(UmiStudioEditorProductivityCommandContribution),       \
        UMI_STUDIO_EDITOR_PRODUCTIVITY_CONTRIBUTION_API_VERSION,                \
        command, menu, group, position, 1                                       \
    }

static const UmiStudioEditorProductivityCommandContribution COMMANDS[] = {
    COMMAND("editor.productivity.format.document", "Edit", "formatting", 500),
    COMMAND("editor.productivity.format.selection", "Edit", "formatting", 510),
    COMMAND("editor.productivity.format.on-save", "Edit", "formatting", 520),
    COMMAND("editor.productivity.format.on-type", "Edit", "formatting", 530),
    COMMAND("editor.productivity.format.select-provider", "Edit", "formatting", 540),
    COMMAND("editor.productivity.imports.organise", "Edit", "imports", 600),
    COMMAND("editor.productivity.imports.remove-unused", "Edit", "imports", 610),
    COMMAND("editor.productivity.imports.sort", "Edit", "imports", 620),
    COMMAND("editor.productivity.snippet.insert", "Edit", "snippets", 700),
    COMMAND("editor.productivity.snippet.next-placeholder", "Navigate", "snippets", 700),
    COMMAND("editor.productivity.snippet.previous-placeholder", "Navigate", "snippets", 710),
    COMMAND("editor.productivity.snippet.cancel", "Edit", "snippets", 710),
    COMMAND("editor.productivity.linked.start", "Edit", "linked-editing", 800),
    COMMAND("editor.productivity.linked.next-range", "Navigate", "linked-editing", 800),
    COMMAND("editor.productivity.linked.apply", "Edit", "linked-editing", 810),
    COMMAND("editor.productivity.linked.cancel", "Edit", "linked-editing", 820)
};

#undef COMMAND

#define VIEW(view, label, contract, region, position)                          \
    {                                                                           \
        (uint32_t)sizeof(UmiStudioEditorProductivityViewContribution),          \
        UMI_STUDIO_EDITOR_PRODUCTIVITY_CONTRIBUTION_API_VERSION,                \
        view, label, contract, region, position, 1, 1                           \
    }

static const UmiStudioEditorProductivityViewContribution VIEWS[] = {
    VIEW("studio.editor.formatting-preview", "Formatting Preview",
         "umicom.editor.formatting-session", "bottom-panel", 80),
    VIEW("studio.editor.snippet-session", "Snippet Session",
         "umicom.editor.snippet-session", "editor-overlay", 20)
};

#undef VIEW

size_t umi_studio_editor_productivity_command_contribution_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiStudioEditorProductivityCommandContribution *
umi_studio_editor_productivity_command_contribution_at(size_t index)
{
    return index < umi_studio_editor_productivity_command_contribution_count()
        ? &COMMANDS[index]
        : NULL;
}

const UmiStudioEditorProductivityCommandContribution *
umi_studio_editor_productivity_command_contribution_find(
    const char *framework_command_id)
{
    size_t index;

    if (framework_command_id == NULL) return NULL;
    for (index = 0U;
         index < umi_studio_editor_productivity_command_contribution_count();
         ++index) {
        if (strcmp(COMMANDS[index].framework_command_id,
                   framework_command_id) == 0) {
            return &COMMANDS[index];
        }
    }
    return NULL;
}

size_t umi_studio_editor_productivity_view_contribution_count(void)
{
    return sizeof(VIEWS) / sizeof(VIEWS[0]);
}

const UmiStudioEditorProductivityViewContribution *
umi_studio_editor_productivity_view_contribution_at(size_t index)
{
    return index < umi_studio_editor_productivity_view_contribution_count()
        ? &VIEWS[index]
        : NULL;
}

const UmiStudioEditorProductivityViewContribution *
umi_studio_editor_productivity_view_contribution_find(const char *view_id)
{
    size_t index;

    if (view_id == NULL) return NULL;
    for (index = 0U;
         index < umi_studio_editor_productivity_view_contribution_count();
         ++index) {
        if (strcmp(VIEWS[index].view_id, view_id) == 0) return &VIEWS[index];
    }
    return NULL;
}
