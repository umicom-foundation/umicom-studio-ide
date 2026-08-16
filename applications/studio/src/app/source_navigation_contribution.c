/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/source_navigation_contribution.c
 *
 * PURPOSE:
 *   Place Framework source-navigation commands in Studio's Navigate and View
 *   menus while Framework remains the sole owner of command behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/source_navigation_contribution.h"

#include <string.h>

#define CONTRIBUTION(command, menu, group, position)                         \
    {                                                                        \
        (uint32_t)sizeof(UmiStudioSourceNavigationContribution),             \
        UMI_STUDIO_SOURCE_NAVIGATION_CONTRIBUTION_API_VERSION,               \
        command, menu, group, position, 1                                    \
    }

static const UmiStudioSourceNavigationContribution CONTRIBUTIONS[] = {
    CONTRIBUTION("editor.navigate.definition", "Navigate", "symbol", 10),
    CONTRIBUTION("editor.navigate.peek-definition", "Navigate", "symbol", 20),
    CONTRIBUTION("editor.navigate.declaration", "Navigate", "symbol", 30),
    CONTRIBUTION("editor.navigate.type-definition", "Navigate", "symbol", 40),
    CONTRIBUTION("editor.navigate.implementation", "Navigate", "symbol", 50),
    CONTRIBUTION("editor.navigate.references", "Navigate", "symbol", 60),
    CONTRIBUTION("editor.navigate.back", "Navigate", "history", 100),
    CONTRIBUTION("editor.navigate.forward", "Navigate", "history", 110),
    CONTRIBUTION("editor.navigate.document-symbols", "Navigate", "search", 200),
    CONTRIBUTION("editor.navigate.workspace-symbols", "Navigate", "search", 210),
    CONTRIBUTION("editor.navigate.focus-breadcrumbs", "View", "navigation", 100),
    CONTRIBUTION("editor.navigate.toggle-outline", "View", "navigation", 110)
};

#undef CONTRIBUTION

size_t umi_studio_source_navigation_contribution_count(void)
{
    return sizeof(CONTRIBUTIONS) / sizeof(CONTRIBUTIONS[0]);
}

const UmiStudioSourceNavigationContribution *
umi_studio_source_navigation_contribution_at(size_t index)
{
    return index < umi_studio_source_navigation_contribution_count()
        ? &CONTRIBUTIONS[index]
        : NULL;
}

const UmiStudioSourceNavigationContribution *
umi_studio_source_navigation_contribution_find(
    const char *framework_command_id)
{
    size_t index;
    if (framework_command_id == NULL) return NULL;
    for (index = 0U;
         index < umi_studio_source_navigation_contribution_count();
         ++index) {
        if (strcmp(CONTRIBUTIONS[index].framework_command_id,
                   framework_command_id) == 0) {
            return &CONTRIBUTIONS[index];
        }
    }
    return NULL;
}
