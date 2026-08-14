/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/helix_agent_views.h
 * PURPOSE: Publish Studio workbench panes for Helix Agent Runtime v2.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Studio supplies pane identity while Framework supplies the
 * toolkit-neutral runtime projection shared by every frontend. */
#ifndef UMICOM_STUDIO_HELIX_AGENT_VIEWS_H
#define UMICOM_STUDIO_HELIX_AGENT_VIEWS_H

#include "umicom/studio/helix_agent_centre.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiStudioHelixAgentPane {
    UMI_STUDIO_HELIX_PANE_OVERVIEW = 1,
    UMI_STUDIO_HELIX_PANE_AGENTS = 2,
    UMI_STUDIO_HELIX_PANE_APPROVALS = 3,
    UMI_STUDIO_HELIX_PANE_JOURNAL = 4,
    UMI_STUDIO_HELIX_PANE_CANDIDATES = 5,
    UMI_STUDIO_HELIX_PANE_ROLLBACK = 6
} UmiStudioHelixAgentPane;

UmiStatus umi_studio_helix_agent_view_create(
    UmiStudioHelixAgentCentre *centre,
    UmiStudioHelixAgentPane pane,
    UmiUiViewModel **out_view);
const char *umi_studio_helix_agent_capability_id(void);

#ifdef __cplusplus
}
#endif

#endif
