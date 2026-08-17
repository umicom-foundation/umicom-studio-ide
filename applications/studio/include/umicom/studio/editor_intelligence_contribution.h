/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/editor_intelligence_contribution.h
 *
 * PURPOSE:
 *   Define Studio placement for Framework-owned editor intelligence, workspace
 *   search, completion, inline suggestions and professional diagnostics without
 *   duplicating reusable implementation in the application.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_EDITOR_INTELLIGENCE_CONTRIBUTION_H
#define UMICOM_STUDIO_EDITOR_INTELLIGENCE_CONTRIBUTION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/completion_command.h"
#include "umicom/editor/intelligence_command.h"
#include "umicom/editor/workspace_search_command.h"
#include "umicom/diagnostics/command.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_EDITOR_INTELLIGENCE_CONTRIBUTION_API_VERSION 3U

typedef enum UmiStudioEditorContributionDomain {
    UMI_STUDIO_EDITOR_CONTRIBUTION_INTELLIGENCE = 1,
    UMI_STUDIO_EDITOR_CONTRIBUTION_WORKSPACE_SEARCH = 2,
    UMI_STUDIO_EDITOR_CONTRIBUTION_COMPLETION = 3,
    UMI_STUDIO_EDITOR_CONTRIBUTION_DIAGNOSTICS = 4
} UmiStudioEditorContributionDomain;

typedef struct UmiStudioEditorIntelligenceCommandContribution {
    uint32_t struct_size;
    uint32_t api_version;
    const char *framework_command_id;
    const char *menu_id;
    const char *menu_group;
    int32_t order;
    int show_in_command_centre;
    UmiStudioEditorContributionDomain domain;
} UmiStudioEditorIntelligenceCommandContribution;

typedef struct UmiStudioEditorIntelligenceViewContribution {
    uint32_t struct_size;
    uint32_t api_version;
    const char *view_id;
    const char *title;
    const char *framework_contract;
    const char *default_region;
    int32_t order;
    int closable;
    int movable;
    UmiStudioEditorContributionDomain domain;
} UmiStudioEditorIntelligenceViewContribution;

size_t umi_studio_editor_intelligence_command_contribution_count(void);
const UmiStudioEditorIntelligenceCommandContribution *
umi_studio_editor_intelligence_command_contribution_at(size_t index);
const UmiStudioEditorIntelligenceCommandContribution *
umi_studio_editor_intelligence_command_contribution_find(
    const char *framework_command_id);
size_t umi_studio_editor_intelligence_view_contribution_count(void);
const UmiStudioEditorIntelligenceViewContribution *
umi_studio_editor_intelligence_view_contribution_at(size_t index);
const UmiStudioEditorIntelligenceViewContribution *
umi_studio_editor_intelligence_view_contribution_find(const char *view_id);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_STUDIO_EDITOR_INTELLIGENCE_CONTRIBUTION_H */
