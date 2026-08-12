/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/workbench_commands.h
 *
 * PURPOSE:
 *   Declare canonical Studio workbench commands so headless, GTK4, menus,
 *   toolbars, keybindings, command palette and automation invoke one implementation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_WORKBENCH_COMMANDS_H
#define UMICOM_STUDIO_WORKBENCH_COMMANDS_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioUi UmiStudioUi;

#define UMI_STUDIO_COMMAND_PERSPECTIVE_ACTIVATE "studio.ui.perspective.activate"
#define UMI_STUDIO_COMMAND_ACTIVITY_ACTIVATE "studio.ui.activity.activate"
#define UMI_STUDIO_COMMAND_PANE_TOGGLE "studio.ui.pane.toggle"
#define UMI_STUDIO_COMMAND_SIDEBAR_TOGGLE "studio.ui.sidebar.toggle"
#define UMI_STUDIO_COMMAND_BOTTOM_PANEL_TOGGLE "studio.ui.bottom-panel.toggle"
#define UMI_STUDIO_COMMAND_AUXILIARY_TOGGLE "studio.ui.auxiliary-sidebar.toggle"
#define UMI_STUDIO_COMMAND_STATUS_SET "studio.ui.status.set"
#define UMI_STUDIO_COMMAND_LAYOUT_RESET "studio.ui.layout.reset"
#define UMI_STUDIO_COMMAND_NOTIFICATION_INFO "studio.ui.notification.info"

UmiStatus umi_studio_workbench_commands_register(UmiCommandRegistry *registry,
                                                  UmiStudioUi *ui);

#ifdef __cplusplus
}
#endif

#endif
