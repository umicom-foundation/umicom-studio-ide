/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/editor_assistance_contribution.c
 *
 * PURPOSE:
 *   Place Framework editor-assistance commands and views in Studio while all
 *   models, validation, navigation and documentation logic stay Framework-owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/editor_assistance_contribution.h"

#include <string.h>

#define COMMAND(command, menu, group, position)                              \
    {                                                                         \
        (uint32_t)sizeof(UmiStudioEditorAssistanceCommandContribution),       \
        UMI_STUDIO_EDITOR_ASSISTANCE_CONTRIBUTION_API_VERSION,                \
        command, menu, group, position, 1                                     \
    }

static const UmiStudioEditorAssistanceCommandContribution COMMANDS[] = {
    COMMAND("editor.assistance.hover.show", "Help", "editor-assistance", 100),
    COMMAND("editor.assistance.hover.pin", "Help", "editor-assistance", 110),
    COMMAND("editor.assistance.hover.dismiss", "Help", "editor-assistance", 120),
    COMMAND("editor.assistance.signature.trigger", "Edit", "intelligence", 400),
    COMMAND("editor.assistance.signature.next", "Edit", "intelligence", 410),
    COMMAND("editor.assistance.signature.previous", "Edit", "intelligence", 420),
    COMMAND("editor.assistance.parameter.next", "Edit", "intelligence", 430),
    COMMAND("editor.assistance.parameter.previous", "Edit", "intelligence", 440),
    COMMAND("editor.assistance.signature.dismiss", "Edit", "intelligence", 450),
    COMMAND("editor.assistance.documentation.show", "Help", "documentation", 200),
    COMMAND("editor.assistance.documentation.pin", "Help", "documentation", 210),
    COMMAND("editor.assistance.documentation.focus", "View", "editor", 360),
    COMMAND("editor.assistance.documentation.open-source", "Help", "documentation", 220),
    COMMAND("editor.assistance.refresh", "View", "editor", 370)
};

#undef COMMAND

#define VIEW(view, label, contract, region, position)                        \
    {                                                                         \
        (uint32_t)sizeof(UmiStudioEditorAssistanceViewContribution),          \
        UMI_STUDIO_EDITOR_ASSISTANCE_CONTRIBUTION_API_VERSION,                \
        view, label, contract, region, position, 1, 1                         \
    }

static const UmiStudioEditorAssistanceViewContribution VIEWS[] = {
    VIEW("studio.editor.signature-help", "Signature Help",
         "umicom.editor.signature-help-model", "editor-overlay", 10),
    VIEW("studio.editor.quick-documentation", "Quick Documentation",
         "umicom.editor.quick-documentation", "secondary-sidebar", 70)
};

#undef VIEW

size_t umi_studio_editor_assistance_command_contribution_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiStudioEditorAssistanceCommandContribution *
umi_studio_editor_assistance_command_contribution_at(size_t index)
{
    return index < umi_studio_editor_assistance_command_contribution_count()
        ? &COMMANDS[index]
        : NULL;
}

const UmiStudioEditorAssistanceCommandContribution *
umi_studio_editor_assistance_command_contribution_find(
    const char *framework_command_id)
{
    size_t index;

    if (framework_command_id == NULL) return NULL;
    for (index = 0U;
         index < umi_studio_editor_assistance_command_contribution_count();
         ++index) {
        if (strcmp(COMMANDS[index].framework_command_id,
                   framework_command_id) == 0) {
            return &COMMANDS[index];
        }
    }
    return NULL;
}

size_t umi_studio_editor_assistance_view_contribution_count(void)
{
    return sizeof(VIEWS) / sizeof(VIEWS[0]);
}

const UmiStudioEditorAssistanceViewContribution *
umi_studio_editor_assistance_view_contribution_at(size_t index)
{
    return index < umi_studio_editor_assistance_view_contribution_count()
        ? &VIEWS[index]
        : NULL;
}

const UmiStudioEditorAssistanceViewContribution *
umi_studio_editor_assistance_view_contribution_find(const char *view_id)
{
    size_t index;

    if (view_id == NULL) return NULL;
    for (index = 0U;
         index < umi_studio_editor_assistance_view_contribution_count();
         ++index) {
        if (strcmp(VIEWS[index].view_id, view_id) == 0) return &VIEWS[index];
    }
    return NULL;
}
