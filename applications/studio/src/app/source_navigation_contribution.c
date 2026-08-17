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
    CONTRIBUTION("editor.navigate.toggle-outline", "View", "navigation", 110),
    CONTRIBUTION("editor.navigate.call-hierarchy", "Navigate", "hierarchy", 300),
    CONTRIBUTION("editor.navigate.incoming-calls", "Navigate", "hierarchy", 310),
    CONTRIBUTION("editor.navigate.outgoing-calls", "Navigate", "hierarchy", 320),
    CONTRIBUTION("editor.navigate.type-hierarchy", "Navigate", "hierarchy", 330),
    CONTRIBUTION("editor.navigate.supertypes", "Navigate", "hierarchy", 340),
    CONTRIBUTION("editor.navigate.subtypes", "Navigate", "hierarchy", 350),
    CONTRIBUTION("editor.navigate.peek-next", "Navigate", "peek", 400),
    CONTRIBUTION("editor.navigate.peek-previous", "Navigate", "peek", 410),
    CONTRIBUTION("editor.navigate.peek-pin", "Navigate", "peek", 420),
    CONTRIBUTION("editor.navigate.peek-close", "Navigate", "peek", 430),
    CONTRIBUTION("editor.navigate.refresh", "Navigate", "session", 500),
    CONTRIBUTION("editor.navigate.cancel", "Navigate", "session", 510),
    CONTRIBUTION("editor.navigate.open-to-side", "Navigate", "open", 600),
    CONTRIBUTION("editor.navigate.open-in-new-window", "Navigate", "open", 610),
    CONTRIBUTION("editor.navigate.copy-location", "Navigate", "copy", 700),
    CONTRIBUTION("editor.navigate.copy-qualified-name", "Navigate", "copy", 710),
    CONTRIBUTION("editor.navigate.reference-next", "Navigate", "references", 800),
    CONTRIBUTION("editor.navigate.reference-previous", "Navigate", "references", 810),
    CONTRIBUTION("editor.navigate.reference-focus-filter", "Navigate", "references", 820),
    CONTRIBUTION("editor.navigate.reference-toggle-declaration", "View", "references", 830),
    CONTRIBUTION("editor.navigate.hierarchy-refresh", "Navigate", "hierarchy-session", 900),
    CONTRIBUTION("editor.navigate.hierarchy-expand-all", "Navigate", "hierarchy-session", 910),
    CONTRIBUTION("editor.navigate.hierarchy-collapse-all", "Navigate", "hierarchy-session", 920),
    CONTRIBUTION("editor.navigate.outline-follow-cursor", "View", "navigation", 120),
    CONTRIBUTION("editor.navigate.breadcrumb-select-sibling", "Navigate", "breadcrumbs", 1000),
    CONTRIBUTION("editor.navigate.provider-diagnostics", "View", "navigation", 130)
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
