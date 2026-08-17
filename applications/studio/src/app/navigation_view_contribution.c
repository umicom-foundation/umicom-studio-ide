/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/navigation_view_contribution.c
 *
 * PURPOSE:
 *   Place Framework source insight surfaces in Studio's workbench while all
 *   reusable state and operations remain owned by Umicom Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/navigation_view_contribution.h"

#include <string.h>

#define VIEW(view, label, contract, region, position)                    \
    {                                                                    \
        (uint32_t)sizeof(UmiStudioNavigationViewContribution),           \
        UMI_STUDIO_NAVIGATION_VIEW_CONTRIBUTION_API_VERSION,             \
        view, label, contract, region, position, 1, 1                    \
    }

static const UmiStudioNavigationViewContribution CONTRIBUTIONS[] = {
    VIEW("studio.navigation.peek-definition", "Peek Definition",
         "umicom.editor.peek-definition", "editor-overlay", 10),
    VIEW("studio.navigation.references", "References",
         "umicom.editor.reference-results", "secondary-sidebar", 20),
    VIEW("studio.navigation.call-hierarchy", "Call Hierarchy",
         "umicom.editor.call-hierarchy", "secondary-sidebar", 30),
    VIEW("studio.navigation.type-hierarchy", "Type Hierarchy",
         "umicom.editor.type-hierarchy", "secondary-sidebar", 40),
    VIEW("studio.navigation.document-outline", "Document Outline",
         "umicom.editor.symbol-navigation-session.document",
         "secondary-sidebar", 50),
    VIEW("studio.navigation.workspace-symbols", "Workspace Symbols",
         "umicom.editor.symbol-navigation-session.workspace",
         "secondary-sidebar", 60),
    VIEW("studio.navigation.source-preview", "Navigation Source Preview",
         "umicom.editor.navigation-source-preview", "editor-overlay", 70),
    VIEW("studio.navigation.providers", "Navigation Providers",
         "umicom.editor.navigation-experience", "secondary-sidebar", 80)
};

#undef VIEW

size_t umi_studio_navigation_view_contribution_count(void)
{
    return sizeof(CONTRIBUTIONS) / sizeof(CONTRIBUTIONS[0]);
}

const UmiStudioNavigationViewContribution *
umi_studio_navigation_view_contribution_at(size_t index)
{
    return index < umi_studio_navigation_view_contribution_count()
        ? &CONTRIBUTIONS[index]
        : NULL;
}

const UmiStudioNavigationViewContribution *
umi_studio_navigation_view_contribution_find(const char *view_id)
{
    size_t index;
    if (view_id == NULL) return NULL;
    for (index = 0U;
         index < umi_studio_navigation_view_contribution_count();
         ++index) {
        if (strcmp(CONTRIBUTIONS[index].view_id, view_id) == 0) {
            return &CONTRIBUTIONS[index];
        }
    }
    return NULL;
}
