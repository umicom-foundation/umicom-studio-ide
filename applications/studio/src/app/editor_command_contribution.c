/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/editor_command_contribution.c
 *
 * PURPOSE:
 *   Map Framework editing commands into Studio's Edit and Selection menus.
 *   Framework remains the sole owner of command identities and behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/editor_command_contribution.h"

#include <string.h>

#define CONTRIBUTION(command, menu, group, position)                  \
    {                                                                \
        (uint32_t)sizeof(UmiStudioEditorCommandContribution),        \
        UMI_STUDIO_EDITOR_COMMAND_CONTRIBUTION_API_VERSION,          \
        command, menu, group, position, 1                            \
    }

static const UmiStudioEditorCommandContribution CONTRIBUTIONS[] = {
    CONTRIBUTION("editor.edit.insert-text", "Edit", "text", 10),
    CONTRIBUTION("editor.edit.replace-range", "Edit", "text", 20),
    CONTRIBUTION("editor.edit.delete-line", "Edit", "lines", 100),
    CONTRIBUTION("editor.edit.duplicate-line", "Edit", "lines", 110),
    CONTRIBUTION("editor.edit.move-line-up", "Edit", "lines", 120),
    CONTRIBUTION("editor.edit.move-line-down", "Edit", "lines", 130),
    CONTRIBUTION("editor.edit.join-line-next", "Edit", "lines", 140),
    CONTRIBUTION("editor.edit.transpose-lines", "Edit", "lines", 150),
    CONTRIBUTION("editor.edit.trim-trailing-whitespace",
                 "Edit", "format", 200),
    CONTRIBUTION("editor.edit.indent-lines", "Selection", "indent", 10),
    CONTRIBUTION("editor.edit.outdent-lines", "Selection", "indent", 20),
    CONTRIBUTION("editor.edit.toggle-line-comment",
                 "Selection", "comments", 100),
    CONTRIBUTION("editor.edit.transform-uppercase",
                 "Selection", "case", 200),
    CONTRIBUTION("editor.edit.transform-lowercase",
                 "Selection", "case", 210)
};

#undef CONTRIBUTION

size_t umi_studio_editor_command_contribution_count(void)
{
    return sizeof(CONTRIBUTIONS) / sizeof(CONTRIBUTIONS[0]);
}

const UmiStudioEditorCommandContribution *
umi_studio_editor_command_contribution_at(size_t index)
{
    return index < umi_studio_editor_command_contribution_count()
        ? &CONTRIBUTIONS[index]
        : NULL;
}

const UmiStudioEditorCommandContribution *
umi_studio_editor_command_contribution_find(const char *framework_command_id)
{
    size_t index;

    if (framework_command_id == NULL) return NULL;
    for (index = 0U;
         index < umi_studio_editor_command_contribution_count();
         ++index) {
        if (strcmp(CONTRIBUTIONS[index].framework_command_id,
                   framework_command_id) == 0) {
            return &CONTRIBUTIONS[index];
        }
    }
    return NULL;
}
