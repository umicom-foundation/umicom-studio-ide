/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/contributions.c
 *
 * PURPOSE:
 *   Populate Studio pane, action, menu, toolbar, status, contribution and
 *   layout models using toolkit-neutral Framework records. Batch 23 also
 *   exposes one primary-sidebar pane for every unified Activity Bar area.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * These records name Studio-specific surfaces. Framework owns how panes are
 * stored, activated, laid out and rendered by headless or GTK4 adapters.
 */

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
    { UMI_STUDIO_PANE_ARCHITECTURE, "Architecture", "studio.architecture", "view-grid-symbolic", UMI_UI_PLACEMENT_RIGHT, 80, 0, 1, 1, { 360, 500 } },

    /*
     * Unified primary-sidebar destinations. These are product-facing hosts;
     * later batches can register richer view-model factories behind the same
     * stable pane/view IDs without changing Activity Bar or layout code.
     */
    { UMI_STUDIO_PANE_SOURCE_CONTROL, "Source Control", "studio.source-control", "org.gnome.Builder-vcs-symbolic", UMI_UI_PLACEMENT_LEFT, 90, 0, 1, 1, { 320, 500 } },
    { UMI_STUDIO_PANE_RUN_DEBUG, "Run and Debug", "studio.run-debug", "system-run-symbolic", UMI_UI_PLACEMENT_LEFT, 100, 0, 1, 1, { 320, 500 } },
    { UMI_STUDIO_PANE_TESTING, "Testing", "studio.testing", "emblem-ok-symbolic", UMI_UI_PLACEMENT_LEFT, 110, 0, 1, 1, { 320, 500 } },
    { UMI_STUDIO_PANE_DESIGNER, "Designer", "studio.designer", "applications-graphics-symbolic", UMI_UI_PLACEMENT_LEFT, 120, 0, 1, 1, { 320, 500 } },
    { UMI_STUDIO_PANE_APPLICATIONS, "Applications", "studio.application-hub", "view-app-grid-symbolic", UMI_UI_PLACEMENT_LEFT, 130, 0, 1, 1, { 320, 500 } },
    { UMI_STUDIO_PANE_FRAMEWORK, "Framework", "studio.framework", "view-grid-symbolic", UMI_UI_PLACEMENT_LEFT, 140, 0, 1, 1, { 320, 500 } },
    { UMI_STUDIO_PANE_AI, "AI / AuthorEngine", "studio.authorengine", "mail-message-new-symbolic", UMI_UI_PLACEMENT_LEFT, 150, 0, 1, 1, { 320, 500 } }
};

static const UmiUiActionSnapshot STUDIO_ACTIONS[] = {
    { "studio.action.perspective.develop", UMI_STUDIO_COMMAND_PERSPECTIVE_ACTIVATE, "Develop", "Activate Develop perspective", "applications-development-symbolic", "Ctrl+1", 1, 1, 0, 0, 10, UMI_STUDIO_PERSPECTIVE_DEVELOP },
    { "studio.action.perspective.build", UMI_STUDIO_COMMAND_PERSPECTIVE_ACTIVATE, "Build", "Activate Build perspective", "system-run-symbolic", "Ctrl+2", 1, 1, 0, 0, 20, UMI_STUDIO_PERSPECTIVE_BUILD },
    { "studio.action.perspective.test", UMI_STUDIO_COMMAND_PERSPECTIVE_ACTIVATE, "Test", "Activate Test perspective", "emblem-ok-symbolic", "Ctrl+3", 1, 1, 0, 0, 30, UMI_STUDIO_PERSPECTIVE_TEST },
    { "studio.action.pane.explorer", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Project Explorer", "Show or hide Project Explorer", "folder-symbolic", "Ctrl+Shift+E", 1, 1, 1, 1, 40, UMI_STUDIO_PANE_EXPLORER },
    { "studio.action.pane.output", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Output", "Show or hide Output", "utilities-terminal-symbolic", "Ctrl+J", 1, 1, 1, 1, 50, UMI_STUDIO_PANE_OUTPUT },
    { "studio.action.pane.problems", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Problems", "Show or hide Problems", "dialog-warning-symbolic", "Ctrl+Shift+M", 1, 1, 1, 1, 60, UMI_STUDIO_PANE_PROBLEMS },
    { "studio.action.layout.reset", UMI_STUDIO_COMMAND_LAYOUT_RESET, "Reset Layout", "Restore the default Studio layout", "view-restore-symbolic", "", 1, 1, 0, 0, 70, "" },
    { "studio.action.notification.info", UMI_STUDIO_COMMAND_NOTIFICATION_INFO, "About This Workbench", "Show Framework workbench information", "dialog-information-symbolic", "", 1, 1, 0, 0, 80, "Umicom Studio 0.17 uses the Framework-owned workbench and Editor Core v2." },

    /*
     * Batch 38 publishes the conventional IDE menu vocabulary immediately.
     * File-system and editor command handlers arrive as subsequent vertical
     * slices; until then these entries report their integration boundary.
     */
    { "studio.action.file.new", UMI_STUDIO_COMMAND_NOTIFICATION_INFO, "New File", "Create an untitled editor document", "document-new-symbolic", "Ctrl+N", 1, 1, 0, 0, 100, "New File is registered; untitled-document persistence is the next editor slice." },
    { "studio.action.file.open", UMI_STUDIO_COMMAND_NOTIFICATION_INFO, "Open File…", "Open a file in the Framework editor", "document-open-symbolic", "Ctrl+O", 1, 1, 0, 0, 110, "Open File is registered; the Framework document-loader adapter is the next editor slice." },
    { "studio.action.file.save", UMI_STUDIO_COMMAND_NOTIFICATION_INFO, "Save", "Save the active Framework working copy", "document-save-symbolic", "Ctrl+S", 1, 1, 0, 0, 120, "Save is registered; the Framework document-saver adapter is the next editor slice." },
    { "studio.action.file.save-as", UMI_STUDIO_COMMAND_NOTIFICATION_INFO, "Save As…", "Save the active document to another URI", "document-save-as-symbolic", "Ctrl+Shift+S", 1, 1, 0, 0, 130, "Save As is registered; URI provider integration is the next editor slice." },
    { "studio.action.edit.undo", UMI_STUDIO_COMMAND_NOTIFICATION_INFO, "Undo", "Undo the latest editor transaction", "edit-undo-symbolic", "Ctrl+Z", 1, 1, 0, 0, 140, "The Framework edit-history service is ready; GTK command routing follows next." },
    { "studio.action.edit.redo", UMI_STUDIO_COMMAND_NOTIFICATION_INFO, "Redo", "Redo the latest editor transaction", "edit-redo-symbolic", "Ctrl+Shift+Z", 1, 1, 0, 0, 150, "The Framework edit-history service is ready; GTK command routing follows next." },
    { "studio.action.edit.find", UMI_STUDIO_COMMAND_NOTIFICATION_INFO, "Find", "Find text in the active editor", "edit-find-symbolic", "Ctrl+F", 1, 1, 0, 0, 160, "The Framework literal-search engine is ready; find-widget binding follows next." },
    { "studio.action.edit.replace", UMI_STUDIO_COMMAND_NOTIFICATION_INFO, "Replace", "Replace text in the active editor", "edit-find-replace-symbolic", "Ctrl+H", 1, 1, 0, 0, 170, "Replace is registered against the Framework editor roadmap." },
    { "studio.action.view.command-palette", UMI_STUDIO_COMMAND_NOTIFICATION_INFO, "Command Palette", "Focus the global Framework command palette", "system-search-symbolic", "Ctrl+Shift+P", 1, 1, 0, 0, 180, "Use the Quick Access field in the workbench toolbar to search commands." },
    { "studio.action.go.line", UMI_STUDIO_COMMAND_NOTIFICATION_INFO, "Go to Line…", "Navigate to a line in the active document", "go-jump-symbolic", "Ctrl+G", 1, 1, 0, 0, 190, "Go to Line is registered against the Framework navigation contracts." },
    { "studio.action.run.build", UMI_STUDIO_COMMAND_PERSPECTIVE_ACTIVATE, "Build Workspace", "Open the Build perspective", "system-run-symbolic", "Ctrl+Shift+B", 1, 1, 0, 0, 200, UMI_STUDIO_PERSPECTIVE_BUILD },
    { "studio.action.run.test", UMI_STUDIO_COMMAND_PERSPECTIVE_ACTIVATE, "Run Tests", "Open the Test perspective", "emblem-ok-symbolic", "Ctrl+Alt+T", 1, 1, 0, 0, 210, UMI_STUDIO_PERSPECTIVE_TEST },
    { "studio.action.terminal.show", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Show Terminal", "Show or hide the terminal pane", "utilities-terminal-symbolic", "Ctrl+`", 1, 1, 1, 0, 220, UMI_STUDIO_PANE_TERMINAL }
};

static const UmiUiMenuSnapshot STUDIO_MENUS[] = {
    { "menu.file.new", "file", "documents", "studio.action.file.new", "", 0, 10 },
    { "menu.file.open", "file", "documents", "studio.action.file.open", "", 0, 20 },
    { "menu.file.sep", "file", "save", "", "", 1, 30 },
    { "menu.file.save", "file", "save", "studio.action.file.save", "", 0, 40 },
    { "menu.file.save-as", "file", "save", "studio.action.file.save-as", "", 0, 50 },
    { "menu.edit.undo", "edit", "history", "studio.action.edit.undo", "", 0, 10 },
    { "menu.edit.redo", "edit", "history", "studio.action.edit.redo", "", 0, 20 },
    { "menu.edit.sep", "edit", "search", "", "", 1, 30 },
    { "menu.edit.find", "edit", "search", "studio.action.edit.find", "", 0, 40 },
    { "menu.edit.replace", "edit", "search", "studio.action.edit.replace", "", 0, 50 },
    { "menu.view.palette", "view", "commands", "studio.action.view.command-palette", "", 0, 10 },
    { "menu.view.explorer", "view", "panes", "studio.action.pane.explorer", "", 0, 20 },
    { "menu.view.output", "view", "panes", "studio.action.pane.output", "", 0, 30 },
    { "menu.view.problems", "view", "panes", "studio.action.pane.problems", "", 0, 40 },
    { "menu.view.reset", "view", "layout", "studio.action.layout.reset", "", 0, 50 },
    { "menu.go.line", "go", "navigation", "studio.action.go.line", "", 0, 10 },
    { "menu.run.build", "run", "build", "studio.action.run.build", "", 0, 10 },
    { "menu.run.test", "run", "test", "studio.action.run.test", "", 0, 20 },
    { "menu.terminal.show", "terminal", "terminal", "studio.action.terminal.show", "", 0, 10 },
    { "menu.help.about", "help", "about", "studio.action.notification.info", "", 0, 10 }
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
    { "studio.contribution.build", "org.umicom.studio.shell", "umicom.ui.perspectives", UMI_STUDIO_PERSPECTIVE_BUILD, 80, 1 },
    { "studio.contribution.source-control", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_SOURCE_CONTROL, 90, 1 },
    { "studio.contribution.run-debug", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_RUN_DEBUG, 100, 1 },
    { "studio.contribution.testing", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_TESTING, 110, 1 },
    { "studio.contribution.designer", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_DESIGNER, 120, 1 },
    { "studio.contribution.applications", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_APPLICATIONS, 130, 1 },
    { "studio.contribution.framework", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_FRAMEWORK, 140, 1 },
    { "studio.contribution.ai", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_AI, 150, 1 }
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

    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_ui_workbench_layout(workbench);
    for (index = 0U; index < sizeof(NODES) / sizeof(NODES[0]); ++index) {
        status = umi_ui_layout_upsert(layout, &NODES[index]);
        if (status != UMI_STATUS_OK) return status;
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

    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
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
