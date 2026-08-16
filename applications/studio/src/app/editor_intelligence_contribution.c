/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/editor_intelligence_contribution.c
 *
 * PURPOSE:
 *   Place Framework editor-intelligence commands and views in Studio while
 *   all reusable state, validation and operations remain Framework-owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/editor_intelligence_contribution.h"

#include <string.h>

#define COMMAND(command, menu, group, position)                              \
    {                                                                         \
        (uint32_t)sizeof(UmiStudioEditorIntelligenceCommandContribution),     \
        UMI_STUDIO_EDITOR_INTELLIGENCE_CONTRIBUTION_API_VERSION,              \
        command, menu, group, position, 1                                     \
    }

static const UmiStudioEditorIntelligenceCommandContribution COMMANDS[] = {
    COMMAND("editor.intelligence.rename", "Edit", "refactor", 300),
    COMMAND("editor.intelligence.rename-preview", "Edit", "refactor", 310),
    COMMAND("editor.intelligence.rename-apply", "Edit", "refactor", 320),
    COMMAND("editor.intelligence.rename-cancel", "Edit", "refactor", 330),
    COMMAND("editor.intelligence.code-lens.toggle", "View", "editor", 300),
    COMMAND("editor.intelligence.code-lens.refresh", "View", "editor", 310),
    COMMAND("editor.intelligence.code-lens.execute", "Navigate", "symbol", 70),
    COMMAND("editor.intelligence.inlay-hints.toggle", "View", "editor", 320),
    COMMAND("editor.intelligence.inlay-hints.refresh", "View", "editor", 330),
    COMMAND("editor.intelligence.semantic.toggle", "View", "editor", 340),
    COMMAND("editor.intelligence.semantic.refresh", "View", "editor", 350),
    COMMAND("editor.intelligence.semantic.inspect", "Navigate", "symbol", 80)
};

#undef COMMAND

#define VIEW(view, label, contract, region, position)                        \
    {                                                                         \
        (uint32_t)sizeof(UmiStudioEditorIntelligenceViewContribution),        \
        UMI_STUDIO_EDITOR_INTELLIGENCE_CONTRIBUTION_API_VERSION,              \
        view, label, contract, region, position, 1, 1                         \
    }

static const UmiStudioEditorIntelligenceViewContribution VIEWS[] = {
    VIEW("studio.editor.rename-preview", "Rename Preview",
         "umicom.editor.rename-symbol", "secondary-sidebar", 50),
    VIEW("studio.editor.semantic-token-inspector", "Semantic Token Inspector",
         "umicom.editor.semantic-highlighting", "bottom-panel", 60)
};

#undef VIEW

size_t umi_studio_editor_intelligence_command_contribution_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiStudioEditorIntelligenceCommandContribution *
umi_studio_editor_intelligence_command_contribution_at(size_t index)
{
    return index <
            umi_studio_editor_intelligence_command_contribution_count()
        ? &COMMANDS[index]
        : NULL;
}

const UmiStudioEditorIntelligenceCommandContribution *
umi_studio_editor_intelligence_command_contribution_find(
    const char *framework_command_id)
{
    size_t index;

    if (framework_command_id == NULL) return NULL;
    for (index = 0U;
         index < umi_studio_editor_intelligence_command_contribution_count();
         ++index) {
        if (strcmp(COMMANDS[index].framework_command_id,
                   framework_command_id) == 0) {
            return &COMMANDS[index];
        }
    }
    return NULL;
}

size_t umi_studio_editor_intelligence_view_contribution_count(void)
{
    return sizeof(VIEWS) / sizeof(VIEWS[0]);
}

const UmiStudioEditorIntelligenceViewContribution *
umi_studio_editor_intelligence_view_contribution_at(size_t index)
{
    return index < umi_studio_editor_intelligence_view_contribution_count()
        ? &VIEWS[index]
        : NULL;
}

const UmiStudioEditorIntelligenceViewContribution *
umi_studio_editor_intelligence_view_contribution_find(const char *view_id)
{
    size_t index;

    if (view_id == NULL) return NULL;
    for (index = 0U;
         index < umi_studio_editor_intelligence_view_contribution_count();
         ++index) {
        if (strcmp(VIEWS[index].view_id, view_id) == 0) return &VIEWS[index];
    }
    return NULL;
}
