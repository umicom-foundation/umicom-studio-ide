/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/editor_intelligence_contribution.c
 *
 * PURPOSE:
 *   Place Framework editor-intelligence, workspace-search, completion, inline-
 *   suggestion and professional diagnostics contracts in Studio. All
 *   operational behaviour remains
 *   authoritative in Umicom Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/editor_intelligence_contribution.h"

#include <string.h>

#define COMMAND(domain_value, command, menu, group, position)                 \
    {                                                                          \
        (uint32_t)sizeof(UmiStudioEditorIntelligenceCommandContribution),      \
        UMI_STUDIO_EDITOR_INTELLIGENCE_CONTRIBUTION_API_VERSION,               \
        command, menu, group, position, 1, domain_value                        \
    }

static const UmiStudioEditorIntelligenceCommandContribution COMMANDS[] = {
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_INTELLIGENCE,
            "editor.intelligence.rename", "Edit", "refactor", 300),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_INTELLIGENCE,
            "editor.intelligence.rename-preview", "Edit", "refactor", 310),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_INTELLIGENCE,
            "editor.intelligence.rename-apply", "Edit", "refactor", 320),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_INTELLIGENCE,
            "editor.intelligence.rename-cancel", "Edit", "refactor", 330),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_INTELLIGENCE,
            "editor.intelligence.code-lens.toggle", "View", "editor", 300),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_INTELLIGENCE,
            "editor.intelligence.code-lens.refresh", "View", "editor", 310),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_INTELLIGENCE,
            "editor.intelligence.code-lens.execute", "Navigate", "symbol", 70),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_INTELLIGENCE,
            "editor.intelligence.inlay-hints.toggle", "View", "editor", 320),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_INTELLIGENCE,
            "editor.intelligence.inlay-hints.refresh", "View", "editor", 330),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_INTELLIGENCE,
            "editor.intelligence.semantic.toggle", "View", "editor", 340),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_INTELLIGENCE,
            "editor.intelligence.semantic.refresh", "View", "editor", 350),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_INTELLIGENCE,
            "editor.intelligence.semantic.inspect", "Navigate", "symbol", 80),

    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "search.workspace.open", "Edit", "find", 100),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "search.workspace.execute", "Edit", "find", 110),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "search.workspace.refresh-index", "Edit", "find", 120),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "search.workspace.next-result", "Edit", "navigation", 200),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "search.workspace.previous-result", "Edit", "navigation", 210),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "search.workspace.toggle-case", "Edit", "search-options", 300),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "search.workspace.toggle-regex", "Edit", "search-options", 310),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "search.workspace.toggle-whole-word", "Edit", "search-options", 320),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "search.workspace.toggle-multiline", "Edit", "search-options", 330),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "search.workspace.toggle-overlap", "Edit", "search-options", 340),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "search.workspace.add-include", "Edit", "search-policy", 400),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "search.workspace.add-exclude", "Edit", "search-policy", 410),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "search.workspace.clear-results", "Edit", "search-results", 500),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "search.workspace.copy-results", "Edit", "search-results", 510),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "search.workspace.export-results", "File", "export", 520),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "replace.workspace.open", "Edit", "replace", 600),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "replace.workspace.build-preview", "Edit", "replace", 610),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "replace.workspace.select-all", "Edit", "replace-selection", 700),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "replace.workspace.clear-selection", "Edit", "replace-selection", 710),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "replace.workspace.prepare", "Edit", "replace-transaction", 800),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "replace.workspace.apply", "Edit", "replace-transaction", 810),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
            "replace.workspace.cancel", "Edit", "replace-transaction", 820),

    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.completion.trigger", "Edit", "completion", 100),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.completion.trigger-suggest", "Edit", "completion", 110),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.completion.hide", "Edit", "completion", 120),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.completion.select-next", "Navigate", "completion", 100),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.completion.select-previous", "Navigate", "completion", 110),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.completion.select-next-page", "Navigate", "completion", 120),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.completion.select-previous-page", "Navigate", "completion", 130),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.completion.accept", "Edit", "completion-accept", 200),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.completion.accept-enter", "Edit", "completion-accept", 210),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.completion.resolve-details", "View", "completion", 300),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.completion.toggle-details", "View", "completion", 310),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.completion.filter-kind", "View", "completion", 320),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.completion.provider-diagnostics", "View", "completion", 330),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.inline-suggestion.trigger", "Edit", "inline-suggestion", 400),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.inline-suggestion.next", "Navigate", "inline-suggestion", 400),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.inline-suggestion.previous", "Navigate", "inline-suggestion", 410),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.inline-suggestion.accept", "Edit", "inline-suggestion", 420),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.inline-suggestion.accept-word", "Edit", "inline-suggestion", 430),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.inline-suggestion.accept-line", "Edit", "inline-suggestion", 440),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.inline-suggestion.reject", "Edit", "inline-suggestion", 450),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.inline-suggestion.toggle", "View", "inline-suggestion", 460),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.inline-suggestion.pause", "View", "inline-suggestion", 470),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.inline-suggestion.ai-enable", "Tools", "ai-suggestions", 500),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
            "editor.inline-suggestion.ai-disable", "Tools", "ai-suggestions", 510),

    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problems.open", "View", "problems", 100),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problems.refresh", "View", "problems", 110),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problems.clear", "View", "problems", 120),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problems.next", "Navigate", "problems", 500),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problems.previous", "Navigate", "problems", 510),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problems.group-file", "View", "problem-grouping", 200),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problems.group-provider", "View", "problem-grouping", 210),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problems.group-severity", "View", "problem-grouping", 220),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problems.filter-errors", "View", "problem-filter", 300),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problems.filter-warnings", "View", "problem-filter", 310),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problems.filter-new", "View", "problem-filter", 320),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problems.filter-suppressed", "View", "problem-filter", 330),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problem.open-details", "View", "problem", 400),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problem.copy-message", "Edit", "problem", 400),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problems.copy-all", "Edit", "problem", 410),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problem.open-fix", "Edit", "quick-fix", 500),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problem.apply-fix", "Edit", "quick-fix", 510),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problem.suppress", "Edit", "suppression", 600),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.problem.unsuppress", "Edit", "suppression", 610),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.suppressions.manage", "Tools", "diagnostics", 600),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.severity-policy.manage", "Tools", "diagnostics", 610),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.baseline.capture", "Tools", "diagnostics", 620),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.baseline.compare", "Tools", "diagnostics", 630),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.sarif.import", "File", "import", 600),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.sarif.export", "File", "export", 610),
    COMMAND(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
            "diagnostics.provider.open-details", "View", "problems", 130)
};

#undef COMMAND

#define VIEW(domain_value, view, label, contract, region, position)           \
    {                                                                          \
        (uint32_t)sizeof(UmiStudioEditorIntelligenceViewContribution),         \
        UMI_STUDIO_EDITOR_INTELLIGENCE_CONTRIBUTION_API_VERSION,               \
        view, label, contract, region, position, 1, 1, domain_value            \
    }

static const UmiStudioEditorIntelligenceViewContribution VIEWS[] = {
    VIEW(UMI_STUDIO_EDITOR_CONTRIBUTION_INTELLIGENCE,
         "studio.editor.rename-preview", "Rename Preview",
         "umicom.editor.rename-symbol", "secondary-sidebar", 50),
    VIEW(UMI_STUDIO_EDITOR_CONTRIBUTION_INTELLIGENCE,
         "studio.editor.semantic-token-inspector", "Semantic Token Inspector",
         "umicom.editor.semantic-highlighting", "bottom-panel", 60),
    VIEW(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
         "studio.search.workspace-results", "Workspace Search",
         "umicom.editor.workspace-search-query", "left-sidebar", 110),
    VIEW(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
         "studio.search.exclusions", "Search Exclusions",
         "umicom.editor.workspace-search-exclusion", "left-sidebar", 120),
    VIEW(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
         "studio.replace.preview", "Replace Preview",
         "umicom.editor.workspace-replacement-preview", "bottom-panel", 150),
    VIEW(UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH,
         "studio.replace.transaction", "Replace Transaction",
         "umicom.editor.workspace-replace-transaction", "bottom-panel", 160),
    VIEW(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
         "studio.editor.completion-details", "Completion Details",
         "umicom.editor.completion-session", "secondary-sidebar", 170),
    VIEW(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
         "studio.editor.completion-providers", "Completion Providers",
         "umicom.editor.completion-orchestration", "bottom-panel", 180),
    VIEW(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
         "studio.editor.inline-suggestion-status", "Inline Suggestion Status",
         "umicom.editor.inline-suggestion-session", "bottom-panel", 190),
    VIEW(UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION,
         "studio.editor.ai-suggestion-policy", "AI Suggestion Policy",
         "umicom.editor.inline-suggestion-provider", "secondary-sidebar", 200),

    VIEW(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
         "studio.diagnostics.problems", "Problems",
         "umicom.diagnostics.problem-model", "bottom-panel", 210),
    VIEW(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
         "studio.diagnostics.problem-details", "Problem Details",
         "umicom.diagnostics.provider-finding", "secondary-sidebar", 220),
    VIEW(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
         "studio.diagnostics.suppressions", "Diagnostic Suppressions",
         "umicom.diagnostics.suppression", "secondary-sidebar", 230),
    VIEW(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
         "studio.diagnostics.severity-policy", "Severity Policy",
         "umicom.diagnostics.severity-policy", "secondary-sidebar", 240),
    VIEW(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
         "studio.diagnostics.baseline", "Diagnostic Baseline",
         "umicom.diagnostics.baseline", "bottom-panel", 250),
    VIEW(UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS,
         "studio.diagnostics.sarif", "SARIF Interchange",
         "umicom.diagnostics.sarif", "bottom-panel", 260)
};

#undef VIEW

size_t umi_studio_editor_intelligence_command_contribution_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiStudioEditorIntelligenceCommandContribution *
umi_studio_editor_intelligence_command_contribution_at(size_t index)
{
    return index < umi_studio_editor_intelligence_command_contribution_count()
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
