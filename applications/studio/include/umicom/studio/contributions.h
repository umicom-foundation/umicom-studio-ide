/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/contributions.h
 *
 * PURPOSE:
 *   Declare Studio contributions for panes, actions, menus, toolbars, status
 *   items and extension-point metadata on the Framework workbench.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

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

UmiStatus umi_studio_contributions_register(UmiUiWorkbench *workbench);
UmiStatus umi_studio_contributions_register_layout(UmiUiWorkbench *workbench);
size_t umi_studio_contribution_definition_count(void);

#ifdef __cplusplus
}
#endif

#endif
