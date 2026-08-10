/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/contributions.c
 *
 * PURPOSE:
 *   Populate Studio pane, action, menu, toolbar, status, contribution and
 *   layout models using toolkit-neutral Framework records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/contributions.h"

#include <string.h>

#include "umicom/studio/perspectives.h"
#include "umicom/studio/workbench_commands.h"

static const UmiUiPaneSnapshot STUDIO_PANES[] = {
    { UMI_STUDIO_PANE_EXPLORER, "Project Explorer", "studio.project-explorer", "folder-symbolic", UMI_UI_PLACEMENT_LEFT, 10, 1, 1, 1, { 280, 500 } },
    { UMI_STUDIO_PANE_EDITOR, "Editor", "studio.editor-host", "text-x-generic-symbolic", UMI_UI_PLACEMENT_CENTRE, 20, 1, 0, 0, { 760, 500 } },
    { UMI_STUDIO_PANE_OUTPUT, "Output", "studio.output", "utilities-terminal-symbolic", UMI_UI_PLACEMENT_BOTTOM, 30, 1, 1, 1, { 760, 220 } },
    { UMI_STUDIO_PANE_PROBLEMS, "Problems", "studio.problems", "dialog-warning-symbolic", UMI_UI_PLACEMENT_BOTTOM, 40, 1, 1, 1, { 760, 220 } },
    { UMI_STUDIO_PANE_SEARCH, "Search", "studio.search", "system-search-symbolic", UMI_UI_PLACEMENT_LEFT, 50, 0, 1, 1, { 320, 500 } },
    { UMI_STUDIO_PANE_TERMINAL, "Terminal", "studio.terminal", "utilities-terminal-symbolic", UMI_UI_PLACEMENT_BOTTOM, 60, 0, 1, 1, { 760, 240 } },
    { UMI_STUDIO_PANE_CHAT, "AI Chat", "studio.ai-chat", "mail-message-new-symbolic", UMI_UI_PLACEMENT_RIGHT, 70, 1, 1, 1, { 360, 500 } },
    { UMI_STUDIO_PANE_ARCHITECTURE, "Architecture", "studio.architecture", "view-grid-symbolic", UMI_UI_PLACEMENT_RIGHT, 80, 0, 1, 1, { 360, 500 } }
};

static const UmiUiActionSnapshot STUDIO_ACTIONS[] = {
    { "studio.action.perspective.develop", UMI_STUDIO_COMMAND_PERSPECTIVE_ACTIVATE, "Develop", "Activate Develop perspective", "applications-development-symbolic", "Ctrl+1", 1, 1, 0, 0, 10 },
    { "studio.action.perspective.build", UMI_STUDIO_COMMAND_PERSPECTIVE_ACTIVATE, "Build", "Activate Build perspective", "system-run-symbolic", "Ctrl+2", 1, 1, 0, 0, 20 },
    { "studio.action.perspective.test", UMI_STUDIO_COMMAND_PERSPECTIVE_ACTIVATE, "Test", "Activate Test perspective", "emblem-ok-symbolic", "Ctrl+3", 1, 1, 0, 0, 30 },
    { "studio.action.pane.explorer", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Project Explorer", "Show or hide Project Explorer", "folder-symbolic", "Ctrl+Shift+E", 1, 1, 1, 1, 40 },
    { "studio.action.pane.output", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Output", "Show or hide Output", "utilities-terminal-symbolic", "Ctrl+J", 1, 1, 1, 1, 50 },
    { "studio.action.pane.problems", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Problems", "Show or hide Problems", "dialog-warning-symbolic", "Ctrl+Shift+M", 1, 1, 1, 1, 60 },
    { "studio.action.layout.reset", UMI_STUDIO_COMMAND_LAYOUT_RESET, "Reset Layout", "Restore the default Studio layout", "view-restore-symbolic", "", 1, 1, 0, 0, 70 },
    { "studio.action.notification.info", UMI_STUDIO_COMMAND_NOTIFICATION_INFO, "Show Notification", "Publish an information notification", "dialog-information-symbolic", "", 1, 1, 0, 0, 80 }
};

static const UmiUiMenuSnapshot STUDIO_MENUS[] = {
    { "menu.view.develop", "view", "perspectives", "studio.action.perspective.develop", "", 0, 10 },
    { "menu.view.build", "view", "perspectives", "studio.action.perspective.build", "", 0, 20 },
    { "menu.view.test", "view", "perspectives", "studio.action.perspective.test", "", 0, 30 },
    { "menu.view.separator", "view", "panes", "", "", 1, 40 },
    { "menu.view.explorer", "view", "panes", "studio.action.pane.explorer", "", 0, 50 },
    { "menu.view.output", "view", "panes", "studio.action.pane.output", "", 0, 60 },
    { "menu.view.problems", "view", "panes", "studio.action.pane.problems", "", 0, 70 },
    { "menu.view.reset", "view", "layout", "studio.action.layout.reset", "", 0, 80 }
};

static const UmiUiToolbarSnapshot STUDIO_TOOLBAR[] = {
    { "toolbar.develop", "main", "studio.action.perspective.develop", "perspectives", 0, 10 },
    { "toolbar.build", "main", "studio.action.perspective.build", "perspectives", 0, 20 },
    { "toolbar.test", "main", "studio.action.perspective.test", "perspectives", 0, 30 },
    { "toolbar.separator", "main", "", "layout", 1, 40 },
    { "toolbar.explorer", "main", "studio.action.pane.explorer", "layout", 0, 50 },
    { "toolbar.output", "main", "studio.action.pane.output", "layout", 0, 60 }
};

static const UmiUiStatusSnapshot STUDIO_STATUS[] = {
    { "studio.status.ready", "Ready", "Studio runtime readiness", "", 100, 1 },
    { "studio.status.workspace", "No workspace", "Current workspace", "", 90, 1 },
    { "studio.status.tasks", "Tasks: 0", "Queued and running tasks", "", 80, 1 },
    { "studio.status.documents", "Documents: 0", "Open documents", "", 70, 1 }
};

static const UmiUiContributionSnapshot STUDIO_CONTRIBUTIONS[] = {
    { "studio.contribution.explorer", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_EXPLORER, 10, 1 },
    { "studio.contribution.editor", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_EDITOR, 20, 1 },
    { "studio.contribution.output", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_OUTPUT, 30, 1 },
    { "studio.contribution.problems", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_PROBLEMS, 40, 1 },
    { "studio.contribution.search", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_SEARCH, 50, 1 },
    { "studio.contribution.chat", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_CHAT, 60, 1 },
    { "studio.contribution.develop", "org.umicom.studio.shell", "umicom.ui.perspectives", UMI_STUDIO_PERSPECTIVE_DEVELOP, 70, 1 },
    { "studio.contribution.build", "org.umicom.studio.shell", "umicom.ui.perspectives", UMI_STUDIO_PERSPECTIVE_BUILD, 80, 1 }
};

UmiStatus umi_studio_contributions_register_layout(UmiUiWorkbench *workbench)
{
    static const UmiUiLayoutNode NODES[] = {
        { "root", "", "", UMI_UI_LAYOUT_ROOT, UMI_UI_HORIZONTAL, 1.0, 0 },
        { "root.horizontal", "root", "", UMI_UI_LAYOUT_SPLIT, UMI_UI_HORIZONTAL, 0.22, 10 },
        { "left.tabs", "root.horizontal", "", UMI_UI_LAYOUT_TABS, UMI_UI_VERTICAL, 1.0, 10 },
        { "left.explorer", "left.tabs", UMI_STUDIO_PANE_EXPLORER, UMI_UI_LAYOUT_PANE, UMI_UI_VERTICAL, 1.0, 10 },
        { "centre.vertical", "root.horizontal", "", UMI_UI_LAYOUT_SPLIT, UMI_UI_VERTICAL, 0.72, 20 },
        { "centre.documents", "centre.vertical", UMI_STUDIO_PANE_EDITOR, UMI_UI_LAYOUT_DOCUMENTS, UMI_UI_VERTICAL, 0.72, 10 },
        { "centre.bottom", "centre.vertical", "", UMI_UI_LAYOUT_TABS, UMI_UI_HORIZONTAL, 0.28, 20 },
        { "bottom.output", "centre.bottom", UMI_STUDIO_PANE_OUTPUT, UMI_UI_LAYOUT_PANE, UMI_UI_HORIZONTAL, 1.0, 10 },
        { "bottom.problems", "centre.bottom", UMI_STUDIO_PANE_PROBLEMS, UMI_UI_LAYOUT_PANE, UMI_UI_HORIZONTAL, 1.0, 20 },
        { "right.chat", "root.horizontal", UMI_STUDIO_PANE_CHAT, UMI_UI_LAYOUT_PANE, UMI_UI_VERTICAL, 0.24, 30 }
    };
    size_t index;
    UmiStatus status;
    UmiUiLayout *layout;

    if (workbench == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    layout = umi_ui_workbench_layout(workbench);
    for (index = 0U; index < sizeof(NODES) / sizeof(NODES[0]); ++index) {
        status = umi_ui_layout_upsert(layout, &NODES[index]);
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    {
        char validation_message[256];
        return umi_ui_layout_validate(layout, validation_message,
                                      sizeof(validation_message));
    }
}

UmiStatus umi_studio_contributions_register(UmiUiWorkbench *workbench)
{
    size_t index;
    UmiStatus status;

    if (workbench == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < sizeof(STUDIO_PANES) / sizeof(STUDIO_PANES[0]); ++index) {
        status = umi_ui_pane_model_upsert(umi_ui_workbench_panes(workbench), &STUDIO_PANES[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    for (index = 0U; index < sizeof(STUDIO_ACTIONS) / sizeof(STUDIO_ACTIONS[0]); ++index) {
        status = umi_ui_action_model_upsert(umi_ui_workbench_actions(workbench), &STUDIO_ACTIONS[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    for (index = 0U; index < sizeof(STUDIO_MENUS) / sizeof(STUDIO_MENUS[0]); ++index) {
        status = umi_ui_menu_model_upsert(umi_ui_workbench_menus(workbench), &STUDIO_MENUS[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    for (index = 0U; index < sizeof(STUDIO_TOOLBAR) / sizeof(STUDIO_TOOLBAR[0]); ++index) {
        status = umi_ui_toolbar_model_upsert(umi_ui_workbench_toolbars(workbench), &STUDIO_TOOLBAR[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    for (index = 0U; index < sizeof(STUDIO_STATUS) / sizeof(STUDIO_STATUS[0]); ++index) {
        status = umi_ui_status_model_upsert(umi_ui_workbench_status(workbench), &STUDIO_STATUS[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    for (index = 0U; index < sizeof(STUDIO_CONTRIBUTIONS) / sizeof(STUDIO_CONTRIBUTIONS[0]); ++index) {
        status = umi_ui_contribution_model_upsert(umi_ui_workbench_contributions(workbench), &STUDIO_CONTRIBUTIONS[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_studio_contributions_register_layout(workbench);
}

size_t umi_studio_contribution_definition_count(void)
{
    return sizeof(STUDIO_CONTRIBUTIONS) / sizeof(STUDIO_CONTRIBUTIONS[0]);
}
