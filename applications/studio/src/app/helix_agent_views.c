/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/helix_agent_views.c
 * PURPOSE: Map Studio pane identifiers to Framework Helix v2 views.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/helix_agent_views.h"

const char *umi_studio_helix_agent_capability_id(void)
{
    return "umicom.studio.helix-agent-runtime";
}

UmiStatus umi_studio_helix_agent_view_create(
    UmiStudioHelixAgentCentre *centre,
    UmiStudioHelixAgentPane pane,
    UmiUiViewModel **out_view)
{
    UmiHelixOrchestrator *runtime =
        umi_studio_helix_agent_centre_runtime(centre);
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    switch (pane) {
        case UMI_STUDIO_HELIX_PANE_OVERVIEW:
            return umi_helix_ui_overview("studio.helix.v2.overview",
                                            runtime, out_view);
        case UMI_STUDIO_HELIX_PANE_AGENTS:
            return umi_helix_ui_agents("studio.helix.v2.agents",
                                          runtime, out_view);
        case UMI_STUDIO_HELIX_PANE_APPROVALS:
            return umi_helix_ui_approvals("studio.helix.v2.approvals",
                                             runtime, out_view);
        case UMI_STUDIO_HELIX_PANE_JOURNAL:
            return umi_helix_ui_journal("studio.helix.v2.journal",
                                           runtime, out_view);
        case UMI_STUDIO_HELIX_PANE_CANDIDATES:
            return umi_helix_ui_candidates("studio.helix.v2.candidates",
                                              runtime, out_view);
        case UMI_STUDIO_HELIX_PANE_ROLLBACK:
            return umi_helix_ui_rollback("studio.helix.v2.rollback",
                                            runtime, out_view);
        default: return UMI_STATUS_INVALID_ARGUMENT;
    }
}
