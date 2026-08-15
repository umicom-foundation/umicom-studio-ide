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
#define UMI_STUDIO_COMMAND_WORKSPACE_PROFILE_ACTIVATE \
    "studio.ui.workspace-profile.activate"
#define UMI_STUDIO_COMMAND_NOTIFICATION_INFO "studio.ui.notification.info"
#define UMI_STUDIO_COMMAND_QUICK_ACCESS_SHOW "studio.ui.quick-access.show"
#define UMI_STUDIO_COMMAND_EDITOR_CLOSE_OTHERS "studio.editor.close-others"
#define UMI_STUDIO_COMMAND_EDITOR_CLOSE_ALL "studio.editor.close-all"
#define UMI_STUDIO_COMMAND_EDITOR_PIN_TOGGLE "studio.editor.pin-toggle"
#define UMI_STUDIO_COMMAND_EDITOR_PREVIEW_PROMOTE "studio.editor.preview-promote"
#define UMI_STUDIO_COMMAND_EDITOR_WORD_WRAP_TOGGLE \
    "studio.editor.word-wrap-toggle"
#define UMI_STUDIO_COMMAND_EDITOR_NEXT "studio.editor.next"
#define UMI_STUDIO_COMMAND_EDITOR_PREVIOUS "studio.editor.previous"
#define UMI_STUDIO_COMMAND_EDITOR_SPLIT_RIGHT "studio.editor.split-right"
#define UMI_STUDIO_COMMAND_EDITOR_SPLIT_DOWN "studio.editor.split-down"
#define UMI_STUDIO_COMMAND_EDITOR_MOVE_NEXT_GROUP \
    "studio.editor.move-next-group"
#define UMI_STUDIO_COMMAND_EDITOR_MOVE_PREVIOUS_GROUP \
    "studio.editor.move-previous-group"
#define UMI_STUDIO_COMMAND_EDITOR_FOCUS_NEXT_GROUP \
    "studio.editor.focus-next-group"
#define UMI_STUDIO_COMMAND_EDITOR_BALANCE_GROUPS \
    "studio.editor.balance-groups"
#define UMI_STUDIO_COMMAND_EDITOR_RESET_GROUPS \
    "studio.editor.reset-groups"

#define UMI_STUDIO_WORKBENCH_COMMAND_COUNT 25U

UmiStatus umi_studio_workbench_commands_register(UmiCommandRegistry *registry,
                                                  UmiStudioUi *ui);

#ifdef __cplusplus
}
#endif

#endif
