/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/contributions.h
 *
 * PURPOSE:
 *   Declare Studio contributions for panes, actions, menus, toolbars, status
 *   items, Activity Bar destinations and extension-point metadata on the
 *   Framework workbench.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Pane IDs are Studio product composition. The pane model, visibility, layout,
 * activity and view-container behaviour remain reusable Umicom Framework code.
 */

#ifndef UMICOM_STUDIO_CONTRIBUTIONS_H
#define UMICOM_STUDIO_CONTRIBUTIONS_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_PANE_EXPLORER "studio.pane.explorer"
#define UMI_STUDIO_PANE_EDITOR "studio.pane.editor"
#define UMI_STUDIO_PANE_OUTPUT "studio.pane.output"
#define UMI_STUDIO_PANE_PROBLEMS "studio.pane.problems"
#define UMI_STUDIO_PANE_SEARCH "studio.pane.search"
#define UMI_STUDIO_PANE_TERMINAL "studio.pane.terminal"
#define UMI_STUDIO_PANE_CHAT "studio.pane.chat"
#define UMI_STUDIO_PANE_ARCHITECTURE "studio.pane.architecture"

/* Batch 23 primary-sidebar views. */
#define UMI_STUDIO_PANE_SOURCE_CONTROL "studio.pane.source-control"
#define UMI_STUDIO_PANE_RUN_DEBUG "studio.pane.run-debug"
#define UMI_STUDIO_PANE_TESTING "studio.pane.testing"
#define UMI_STUDIO_PANE_DESIGNER "studio.pane.designer"
#define UMI_STUDIO_PANE_APPLICATIONS "studio.pane.applications"
#define UMI_STUDIO_PANE_FRAMEWORK "studio.pane.framework"
#define UMI_STUDIO_PANE_AI "studio.pane.ai"

UmiStatus umi_studio_contributions_register(UmiUiWorkbench *workbench);
UmiStatus umi_studio_contributions_register_layout(UmiUiWorkbench *workbench);
size_t umi_studio_contribution_definition_count(void);

#ifdef __cplusplus
}
#endif

#endif
