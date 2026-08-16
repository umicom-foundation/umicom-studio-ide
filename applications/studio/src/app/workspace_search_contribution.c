/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/workspace_search_contribution.c
 *
 * PURPOSE:
 *   Place Framework workspace-search commands and view contracts in the Studio
 *   product shell. All operational behaviour remains in Umicom Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/workspace_search_contribution.h"

#include <string.h>

#define COMMAND(command, menu, group, position)                               \
    {                                                                          \
        (uint32_t)sizeof(UmiStudioWorkspaceSearchCommandContribution),         \
        UMI_STUDIO_WORKSPACE_SEARCH_CONTRIBUTION_API_VERSION,                   \
        command, menu, group, position, 1                                      \
    }

static const UmiStudioWorkspaceSearchCommandContribution COMMANDS[] = {
    COMMAND("search.workspace.open", "Edit", "find", 100),
    COMMAND("search.workspace.execute", "Edit", "find", 110),
    COMMAND("search.workspace.refresh-index", "Edit", "find", 120),
    COMMAND("search.workspace.next-result", "Edit", "navigation", 200),
    COMMAND("search.workspace.previous-result", "Edit", "navigation", 210),
    COMMAND("search.workspace.toggle-case", "Edit", "search-options", 300),
    COMMAND("search.workspace.toggle-regex", "Edit", "search-options", 310),
    COMMAND("search.workspace.toggle-whole-word", "Edit", "search-options", 320),
    COMMAND("search.workspace.toggle-multiline", "Edit", "search-options", 330),
    COMMAND("search.workspace.toggle-overlap", "Edit", "search-options", 340),
    COMMAND("search.workspace.add-include", "Edit", "search-policy", 400),
    COMMAND("search.workspace.add-exclude", "Edit", "search-policy", 410),
    COMMAND("search.workspace.clear-results", "Edit", "search-results", 500),
    COMMAND("search.workspace.copy-results", "Edit", "search-results", 510),
    COMMAND("search.workspace.export-results", "File", "export", 520),
    COMMAND("replace.workspace.open", "Edit", "replace", 600),
    COMMAND("replace.workspace.build-preview", "Edit", "replace", 610),
    COMMAND("replace.workspace.select-all", "Edit", "replace-selection", 700),
    COMMAND("replace.workspace.clear-selection", "Edit", "replace-selection", 710),
    COMMAND("replace.workspace.prepare", "Edit", "replace-transaction", 800),
    COMMAND("replace.workspace.apply", "Edit", "replace-transaction", 810),
    COMMAND("replace.workspace.cancel", "Edit", "replace-transaction", 820)
};

#undef COMMAND

#define VIEW(view, label, contract, region, position)                         \
    {                                                                          \
        (uint32_t)sizeof(UmiStudioWorkspaceSearchViewContribution),            \
        UMI_STUDIO_WORKSPACE_SEARCH_CONTRIBUTION_API_VERSION,                   \
        view, label, contract, region, position, 1, 1                          \
    }

static const UmiStudioWorkspaceSearchViewContribution VIEWS[] = {
    VIEW("studio.search.workspace-results", "Workspace Search",
         "umicom.editor.workspace-search-query", "left-sidebar", 110),
    VIEW("studio.search.exclusions", "Search Exclusions",
         "umicom.editor.workspace-search-exclusion", "left-sidebar", 120),
    VIEW("studio.replace.preview", "Replace Preview",
         "umicom.editor.workspace-replacement-preview", "bottom-panel", 150),
    VIEW("studio.replace.transaction", "Replace Transaction",
         "umicom.editor.workspace-replace-transaction", "bottom-panel", 160)
};

#undef VIEW

size_t umi_studio_workspace_search_command_contribution_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiStudioWorkspaceSearchCommandContribution *
umi_studio_workspace_search_command_contribution_at(size_t position)
{
    return position < umi_studio_workspace_search_command_contribution_count()
        ? &COMMANDS[position] : NULL;
}

const UmiStudioWorkspaceSearchCommandContribution *
umi_studio_workspace_search_command_contribution_find(
    const char *framework_command_id)
{
    size_t position;
    if (framework_command_id == NULL) return NULL;
    for (position = 0U;
         position < umi_studio_workspace_search_command_contribution_count();
         ++position) {
        if (strcmp(COMMANDS[position].framework_command_id,
                   framework_command_id) == 0) {
            return &COMMANDS[position];
        }
    }
    return NULL;
}

size_t umi_studio_workspace_search_view_contribution_count(void)
{
    return sizeof(VIEWS) / sizeof(VIEWS[0]);
}

const UmiStudioWorkspaceSearchViewContribution *
umi_studio_workspace_search_view_contribution_at(size_t position)
{
    return position < umi_studio_workspace_search_view_contribution_count()
        ? &VIEWS[position] : NULL;
}

const UmiStudioWorkspaceSearchViewContribution *
umi_studio_workspace_search_view_contribution_find(const char *view_id)
{
    size_t position;
    if (view_id == NULL) return NULL;
    for (position = 0U;
         position < umi_studio_workspace_search_view_contribution_count();
         ++position) {
        if (strcmp(VIEWS[position].view_id, view_id) == 0) {
            return &VIEWS[position];
        }
    }
    return NULL;
}
