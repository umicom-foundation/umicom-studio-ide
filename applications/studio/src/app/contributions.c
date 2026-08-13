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

#include "umicom/document/commands.h"
#include "umicom/studio/commands.h"
#include "umicom/studio/perspectives.h"
#include "umicom/studio/workbench_commands.h"

static const UmiUiPaneSnapshot STUDIO_PANES[] = {
    { UMI_STUDIO_PANE_EXPLORER, "Project Explorer", "studio.project-explorer", "folder-symbolic", UMI_UI_PLACEMENT_LEFT, 10, 1, 1, 1, { 280, 500 } },
    { UMI_STUDIO_PANE_EDITOR, "Editor", "studio.editor-host", "text-x-generic-symbolic", UMI_UI_PLACEMENT_CENTRE, 20, 1, 0, 0, { 760, 500 } },
    { UMI_STUDIO_PANE_OUTPUT, "Output", "studio.output", "utilities-terminal-symbolic", UMI_UI_PLACEMENT_BOTTOM, 30, 1, 1, 1, { 760, 220 } },
    { UMI_STUDIO_PANE_PROBLEMS, "Problems", "studio.problems", "dialog-warning-symbolic", UMI_UI_PLACEMENT_BOTTOM, 40, 1, 1, 1, { 760, 220 } },
    { UMI_STUDIO_PANE_SEARCH, "Search", "studio.search", "system-search-symbolic", UMI_UI_PLACEMENT_LEFT, 50, 0, 1, 1, { 320, 500 } },
    { UMI_STUDIO_PANE_TERMINAL, "Terminal", "studio.terminal", "utilities-terminal-symbolic", UMI_UI_PLACEMENT_BOTTOM, 60, 1, 1, 1, { 760, 240 } },
    { UMI_STUDIO_PANE_PROCESSES, "Processes", "studio.processes", "system-run-symbolic", UMI_UI_PLACEMENT_BOTTOM, 62, 0, 1, 1, { 760, 240 } },
    { UMI_STUDIO_PANE_TASKS, "Tasks", "studio.tasks", "view-list-symbolic", UMI_UI_PLACEMENT_BOTTOM, 64, 0, 1, 1, { 760, 240 } },
    { UMI_STUDIO_PANE_TERMINAL_HISTORY, "Terminal History", "studio.terminal-history", "document-open-recent-symbolic", UMI_UI_PLACEMENT_BOTTOM, 66, 0, 1, 1, { 760, 240 } },
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
    { "studio.action.perspective.develop", UMI_STUDIO_COMMAND_PERSPECTIVE_ACTIVATE, "Develop", "Activate Develop perspective", "applications-development-symbolic", "Ctrl+1", 1, 1, 0, 0, 10, UMI_STUDIO_PERSPECTIVE_DEVELOP, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.perspective.build", UMI_STUDIO_COMMAND_PERSPECTIVE_ACTIVATE, "Build", "Activate Build perspective", "system-run-symbolic", "Ctrl+2", 1, 1, 0, 0, 20, UMI_STUDIO_PERSPECTIVE_BUILD, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.perspective.test", UMI_STUDIO_COMMAND_PERSPECTIVE_ACTIVATE, "Test", "Activate Test perspective", "emblem-ok-symbolic", "Ctrl+3", 1, 1, 0, 0, 30, UMI_STUDIO_PERSPECTIVE_TEST, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.explorer", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Project Explorer", "Show or hide Project Explorer", "folder-symbolic", "Ctrl+Shift+E", 1, 1, 1, 1, 40, UMI_STUDIO_PANE_EXPLORER, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.output", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Output", "Show or hide Output", "utilities-terminal-symbolic", "Ctrl+J", 1, 1, 1, 1, 50, UMI_STUDIO_PANE_OUTPUT, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.problems", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Problems", "Show or hide Problems", "dialog-warning-symbolic", "Ctrl+Shift+M", 1, 1, 1, 1, 60, UMI_STUDIO_PANE_PROBLEMS, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.layout.reset", UMI_STUDIO_COMMAND_LAYOUT_RESET, "Reset Layout", "Restore the default Studio layout", "view-restore-symbolic", "", 1, 1, 0, 0, 70, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.notification.info", UMI_STUDIO_COMMAND_NOTIFICATION_INFO, "About This Workbench", "Show Framework workbench information", "dialog-information-symbolic", "", 1, 1, 0, 0, 80, "Umicom Studio 0.17 uses the Framework-owned workbench and Editor Core v2.", UMI_UI_ACTION_ARGUMENT_NONE },

    /* Batch 39 binds the visible IDE vocabulary to canonical Framework
     * document commands. GTK only collects arguments; Framework owns logic. */
    { "studio.action.file.new", UMI_DOCUMENT_COMMAND_NEW, "New File", "Create an untitled editor document", "document-new-symbolic", "Ctrl+N", 1, 1, 0, 0, 100, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.file.open", UMI_DOCUMENT_COMMAND_OPEN, "Open File…", "Open a file in the Framework editor", "document-open-symbolic", "Ctrl+O", 1, 1, 0, 0, 110, "", UMI_UI_ACTION_ARGUMENT_OPEN_PATH },
    { "studio.action.file.save", UMI_DOCUMENT_COMMAND_SAVE, "Save", "Save the active Framework working copy", "document-save-symbolic", "Ctrl+S", 1, 1, 0, 0, 120, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.file.save-as", UMI_DOCUMENT_COMMAND_SAVE_AS, "Save As…", "Save the active document to another path", "document-save-as-symbolic", "Ctrl+Shift+S", 1, 1, 0, 0, 130, "", UMI_UI_ACTION_ARGUMENT_SAVE_PATH },
    { "studio.action.file.close", UMI_DOCUMENT_COMMAND_CLOSE, "Close File", "Close the active document without data loss", "window-close-symbolic", "Ctrl+W", 1, 1, 0, 0, 135, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.edit.undo", UMI_DOCUMENT_COMMAND_UNDO, "Undo", "Undo the latest editor transaction", "edit-undo-symbolic", "Ctrl+Z", 1, 1, 0, 0, 140, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.edit.redo", UMI_DOCUMENT_COMMAND_REDO, "Redo", "Redo the latest editor transaction", "edit-redo-symbolic", "Ctrl+Shift+Z", 1, 1, 0, 0, 150, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.edit.find", UMI_DOCUMENT_COMMAND_FIND, "Find", "Find text in the active editor", "edit-find-symbolic", "Ctrl+F", 1, 1, 0, 0, 160, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.edit.replace", UMI_DOCUMENT_COMMAND_REPLACE, "Replace", "Replace text in the active editor", "edit-find-replace-symbolic", "Ctrl+H", 1, 1, 0, 0, 170, "", UMI_UI_ACTION_ARGUMENT_FIND_REPLACE },
    { "studio.action.view.command-palette", UMI_STUDIO_COMMAND_NOTIFICATION_INFO, "Command Palette", "Focus the global Framework command palette", "system-search-symbolic", "Ctrl+Shift+P", 1, 1, 0, 0, 180, "Use the Quick Access field in the workbench toolbar to search commands.", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.go.line", UMI_DOCUMENT_COMMAND_GO_TO_LINE, "Go to Line…", "Navigate to a line in the active document", "go-jump-symbolic", "Ctrl+G", 1, 1, 0, 0, 190, "", UMI_UI_ACTION_ARGUMENT_LINE_NUMBER },
    { "studio.action.build.configure", UMI_STUDIO_COMMAND_BUILD_CONFIGURE, "Configure Workspace", "Configure the active CMake build profile", "emblem-system-symbolic", "", 1, 1, 0, 0, 200, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.build.compile", UMI_STUDIO_COMMAND_BUILD_COMPILE, "Build Workspace", "Compile the active CMake build profile", "system-run-symbolic", "Ctrl+Shift+B", 1, 1, 0, 0, 210, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.build.test", UMI_STUDIO_COMMAND_BUILD_TEST, "Run Tests", "Run all CTest tests in the active profile", "emblem-ok-symbolic", "Ctrl+Alt+T", 1, 1, 0, 0, 220, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.build.clean", UMI_STUDIO_COMMAND_BUILD_CLEAN, "Clean Workspace", "Clean generated outputs in the active profile", "edit-clear-all-symbolic", "", 1, 1, 0, 0, 230, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.build.run", UMI_STUDIO_COMMAND_BUILD_RUN, "Start Without Debugging", "Start the configured Studio executable without a debug adapter", "media-playback-start-symbolic", "Ctrl+F5", 1, 1, 0, 0, 240, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.build.install", UMI_STUDIO_COMMAND_BUILD_INSTALL, "Deploy Local", "Install into the configured local staging prefix", "package-x-generic-symbolic", "", 1, 1, 0, 0, 250, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.test.discover", UMI_STUDIO_COMMAND_TESTS_DISCOVER, "Discover Tests", "Discover CTest tests in the active build directory", "system-search-symbolic", "", 1, 1, 0, 0, 260, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.terminal.show", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Show Terminal", "Show or hide the terminal pane", "utilities-terminal-symbolic", "Ctrl+`", 1, 1, 1, 1, 270, UMI_STUDIO_PANE_TERMINAL, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.terminal.execute", UMI_STUDIO_COMMAND_TERMINAL_EXECUTE, "Execute in Terminal…", "Execute a command in the primary terminal", "utilities-terminal-symbolic", "", 1, 1, 0, 0, 280, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.terminal.clear", UMI_STUDIO_COMMAND_TERMINAL_CLEAR, "Clear Terminal", "Clear the primary terminal transcript", "edit-clear-all-symbolic", "", 1, 1, 0, 0, 290, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.terminal.new", UMI_STUDIO_COMMAND_TERMINAL_NEW, "New Terminal", "Open a terminal using the default profile", "tab-new-symbolic", "Ctrl+Shift+`", 1, 1, 0, 0, 291, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.terminal.close", UMI_STUDIO_COMMAND_TERMINAL_CLOSE, "Close Terminal", "Close the active terminal", "window-close-symbolic", "", 1, 1, 0, 0, 292, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.terminal.next", UMI_STUDIO_COMMAND_TERMINAL_NEXT, "Next Terminal", "Activate the next terminal tab", "go-next-symbolic", "", 1, 1, 0, 0, 293, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.terminal.previous", UMI_STUDIO_COMMAND_TERMINAL_PREVIOUS, "Previous Terminal", "Activate the previous terminal tab", "go-previous-symbolic", "", 1, 1, 0, 0, 294, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.terminal.split", UMI_STUDIO_COMMAND_TERMINAL_SPLIT_HORIZONTAL, "Split Terminal", "Split the active terminal horizontally", "view-split-left-right-symbolic", "", 1, 1, 0, 0, 295, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.terminal.split-horizontal", UMI_STUDIO_COMMAND_TERMINAL_SPLIT_HORIZONTAL, "Split Horizontally", "Split the active terminal horizontally", "view-split-left-right-symbolic", "", 1, 1, 0, 0, 296, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.terminal.split-vertical", UMI_STUDIO_COMMAND_TERMINAL_SPLIT_VERTICAL, "Split Vertically", "Split the active terminal vertically", "view-split-top-bottom-symbolic", "", 1, 1, 0, 0, 297, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.terminal.search", UMI_STUDIO_COMMAND_TERMINAL_SEARCH, "Search Terminal…", "Search the active terminal transcript", "edit-find-symbolic", "Ctrl+Shift+F", 1, 1, 0, 0, 298, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.terminal.history-clear", UMI_STUDIO_COMMAND_TERMINAL_HISTORY_CLEAR, "Clear Terminal History", "Clear retained command history", "edit-clear-all-symbolic", "", 1, 1, 0, 0, 299, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.processes", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Processes", "Show or hide supervised processes", "system-run-symbolic", "", 1, 1, 1, 1, 300, UMI_STUDIO_PANE_PROCESSES, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.tasks", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Tasks", "Show or hide background tasks", "view-list-symbolic", "", 1, 1, 1, 1, 301, UMI_STUDIO_PANE_TASKS, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.terminal-history", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Terminal History", "Show or hide terminal command history", "document-open-recent-symbolic", "", 1, 1, 1, 1, 302, UMI_STUDIO_PANE_TERMINAL_HISTORY, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.process.refresh", UMI_STUDIO_COMMAND_PROCESS_REPORT, "Refresh Processes", "Refresh supervised process state", "view-refresh-symbolic", "", 1, 1, 0, 0, 303, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.process.cancel", UMI_STUDIO_COMMAND_PROCESS_CANCEL, "Cancel Process…", "Cancel a supervised process by ID", "process-stop-symbolic", "", 1, 1, 0, 0, 304, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.tasks.refresh", UMI_STUDIO_COMMAND_TASKS_REPORT, "Refresh Tasks", "Refresh shared task queue state", "view-refresh-symbolic", "", 1, 1, 0, 0, 305, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.tasks.wait", UMI_STUDIO_COMMAND_TASKS_WAIT_IDLE, "Wait for Tasks", "Wait for queued tasks to become idle", "media-playback-pause-symbolic", "", 1, 1, 0, 0, 306, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.tasks.retry", UMI_STUDIO_COMMAND_BUILD_RETRY, "Retry Operation…", "Retry a failed build operation node", "view-refresh-symbolic", "", 1, 1, 0, 0, 307, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.tasks.cancel", UMI_STUDIO_COMMAND_BUILD_CANCEL, "Cancel Build", "Cancel the active build operation", "process-stop-symbolic", "", 1, 1, 0, 0, 308, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.problems.clear", UMI_STUDIO_COMMAND_DIAGNOSTICS_CLEAR, "Clear Problems", "Clear retained compiler, language and runtime problems", "edit-clear-all-symbolic", "", 1, 1, 0, 0, 292, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.output.clear", UMI_STUDIO_COMMAND_OUTPUT_CLEAR, "Clear Output", "Clear retained build, terminal and runtime output", "edit-clear-all-symbolic", "", 1, 1, 0, 0, 294, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.language.initialize", UMI_STUDIO_COMMAND_LANGUAGE_INITIALIZE, "Start Language Intelligence", "Initialise the active Language Server Protocol client", "accessories-dictionary-symbolic", "", 1, 1, 0, 0, 300, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.language.workspace-symbols", UMI_STUDIO_COMMAND_LANGUAGE_WORKSPACE_SYMBOLS, "Go to Symbol in Workspace…", "Search workspace symbols through the active language server", "system-search-symbolic", "Ctrl+T", 1, 1, 0, 0, 310, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.debug.start", UMI_STUDIO_COMMAND_DEBUG_START, "Start Debugging", "Launch the configured program through the active debug adapter", "debug-run-symbolic", "F5", 1, 1, 0, 0, 320, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.debug.continue", UMI_STUDIO_COMMAND_DEBUG_CONTINUE, "Continue", "Continue the selected debug thread", "media-playback-start-symbolic", "F6", 1, 1, 0, 0, 330, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.debug.pause", UMI_STUDIO_COMMAND_DEBUG_PAUSE, "Pause", "Pause the selected debug thread", "media-playback-pause-symbolic", "", 1, 1, 0, 0, 340, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.debug.next", UMI_STUDIO_COMMAND_DEBUG_NEXT, "Step Over", "Step over the next statement", "go-next-symbolic", "F10", 1, 1, 0, 0, 350, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.debug.step-in", UMI_STUDIO_COMMAND_DEBUG_STEP_IN, "Step Into", "Step into the next function call", "go-down-symbolic", "F11", 1, 1, 0, 0, 360, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.debug.step-out", UMI_STUDIO_COMMAND_DEBUG_STEP_OUT, "Step Out", "Step out of the current function", "go-up-symbolic", "Shift+F11", 1, 1, 0, 0, 370, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.debug.stop", UMI_STUDIO_COMMAND_DEBUG_STOP, "Stop Debugging", "Terminate the active debug session", "media-playback-stop-symbolic", "Shift+F5", 1, 1, 0, 0, 380, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.debug.add-breakpoint", UMI_STUDIO_COMMAND_DEBUG_ADD_BREAKPOINT, "Add Breakpoint…", "Add a Framework-owned source breakpoint using path:line", "media-record-symbolic", "F9", 1, 1, 0, 0, 390, "", UMI_UI_ACTION_ARGUMENT_TEXT }
};

static const UmiUiMenuSnapshot STUDIO_MENUS[] = {
    { "menu.file.new", "file", "documents", "studio.action.file.new", "", 0, 10 },
    { "menu.file.open", "file", "documents", "studio.action.file.open", "", 0, 20 },
    { "menu.file.sep", "file", "save", "", "", 1, 30 },
    { "menu.file.save", "file", "save", "studio.action.file.save", "", 0, 40 },
    { "menu.file.save-as", "file", "save", "studio.action.file.save-as", "", 0, 50 },
    { "menu.file.close", "file", "documents", "studio.action.file.close", "", 0, 60 },
    { "menu.edit.undo", "edit", "history", "studio.action.edit.undo", "", 0, 10 },
    { "menu.edit.redo", "edit", "history", "studio.action.edit.redo", "", 0, 20 },
    { "menu.edit.sep", "edit", "search", "", "", 1, 30 },
    { "menu.edit.find", "edit", "search", "studio.action.edit.find", "", 0, 40 },
    { "menu.edit.replace", "edit", "search", "studio.action.edit.replace", "", 0, 50 },
    { "menu.view.palette", "view", "commands", "studio.action.view.command-palette", "", 0, 10 },
    { "menu.view.explorer", "view", "panes", "studio.action.pane.explorer", "", 0, 20 },
    { "menu.view.output", "view", "panes", "studio.action.pane.output", "", 0, 30 },
    { "menu.view.problems", "view", "panes", "studio.action.pane.problems", "", 0, 40 },
    { "menu.view.processes", "view", "panes", "studio.action.pane.processes", "", 0, 42 },
    { "menu.view.tasks", "view", "panes", "studio.action.pane.tasks", "", 0, 44 },
    { "menu.view.terminal-history", "view", "panes", "studio.action.pane.terminal-history", "", 0, 46 },
    { "menu.view.clear-output", "view", "diagnostics", "studio.action.output.clear", "", 0, 50 },
    { "menu.view.clear-problems", "view", "diagnostics", "studio.action.problems.clear", "", 0, 60 },
    { "menu.view.reset", "view", "layout", "studio.action.layout.reset", "", 0, 70 },
    { "menu.go.line", "go", "navigation", "studio.action.go.line", "", 0, 10 },
    { "menu.go.workspace-symbols", "go", "navigation", "studio.action.language.workspace-symbols", "", 0, 20 },
    { "menu.run.configure", "run", "build", "studio.action.build.configure", "", 0, 10 },
    { "menu.run.build", "run", "build", "studio.action.build.compile", "", 0, 20 },
    { "menu.run.test", "run", "test", "studio.action.build.test", "", 0, 30 },
    { "menu.run.clean", "run", "build", "studio.action.build.clean", "", 0, 40 },
    { "menu.run.start", "run", "run", "studio.action.build.run", "", 0, 50 },
    { "menu.run.deploy", "run", "deploy", "studio.action.build.install", "", 0, 60 },
    { "menu.run.debug-separator", "run", "debug", "", "", 1, 70 },
    { "menu.run.debug-start", "run", "debug", "studio.action.debug.start", "", 0, 80 },
    { "menu.run.debug-continue", "run", "debug", "studio.action.debug.continue", "", 0, 90 },
    { "menu.run.debug-pause", "run", "debug", "studio.action.debug.pause", "", 0, 100 },
    { "menu.run.debug-next", "run", "debug", "studio.action.debug.next", "", 0, 110 },
    { "menu.run.debug-step-in", "run", "debug", "studio.action.debug.step-in", "", 0, 120 },
    { "menu.run.debug-step-out", "run", "debug", "studio.action.debug.step-out", "", 0, 130 },
    { "menu.run.debug-stop", "run", "debug", "studio.action.debug.stop", "", 0, 140 },
    { "menu.run.debug-breakpoint", "run", "debug", "studio.action.debug.add-breakpoint", "", 0, 150 },
    { "menu.terminal.show", "terminal", "terminal", "studio.action.terminal.show", "", 0, 10 },
    { "menu.terminal.new", "terminal", "sessions", "studio.action.terminal.new", "", 0, 20 },
    { "menu.terminal.close", "terminal", "sessions", "studio.action.terminal.close", "", 0, 30 },
    { "menu.terminal.next", "terminal", "sessions", "studio.action.terminal.next", "", 0, 40 },
    { "menu.terminal.previous", "terminal", "sessions", "studio.action.terminal.previous", "", 0, 50 },
    { "menu.terminal.execute", "terminal", "commands", "studio.action.terminal.execute", "", 0, 60 },
    { "menu.terminal.search", "terminal", "commands", "studio.action.terminal.search", "", 0, 70 },
    { "menu.terminal.clear", "terminal", "commands", "studio.action.terminal.clear", "", 0, 80 },
    { "menu.terminal.split-horizontal", "terminal", "layout", "studio.action.terminal.split-horizontal", "", 0, 90 },
    { "menu.terminal.split-vertical", "terminal", "layout", "studio.action.terminal.split-vertical", "", 0, 100 },
    { "menu.terminal.history-clear", "terminal", "history", "studio.action.terminal.history-clear", "", 0, 110 },
    { "menu.help.about", "help", "about", "studio.action.notification.info", "", 0, 10 }
};

static const UmiUiToolbarSnapshot STUDIO_TOOLBAR[] = {
    { "toolbar.develop", "main", "studio.action.perspective.develop", "perspectives", 0, 10 },
    { "toolbar.build", "main", "studio.action.perspective.build", "perspectives", 0, 20 },
    { "toolbar.test", "main", "studio.action.perspective.test", "perspectives", 0, 30 },
    { "toolbar.separator", "main", "", "layout", 1, 40 },
    { "toolbar.explorer", "main", "studio.action.pane.explorer", "layout", 0, 50 },
    { "toolbar.output", "main", "studio.action.pane.output", "layout", 0, 60 },
    { "toolbar.run-separator", "main", "", "run", 1, 70 },
    { "toolbar.debug-start", "main", "studio.action.debug.start", "run", 0, 80 },
    { "toolbar.debug-stop", "main", "studio.action.debug.stop", "run", 0, 90 }
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
    { "studio.contribution.terminal", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_TERMINAL, 55, 1 },
    { "studio.contribution.processes", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_PROCESSES, 56, 1 },
    { "studio.contribution.tasks", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_TASKS, 57, 1 },
    { "studio.contribution.terminal-history", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_TERMINAL_HISTORY, 58, 1 },
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
        { "bottom.terminal", "centre.bottom", UMI_STUDIO_PANE_TERMINAL, UMI_UI_LAYOUT_PANE, UMI_UI_HORIZONTAL, 1.0, 30 },
        { "bottom.processes", "centre.bottom", UMI_STUDIO_PANE_PROCESSES, UMI_UI_LAYOUT_PANE, UMI_UI_HORIZONTAL, 1.0, 40 },
        { "bottom.tasks", "centre.bottom", UMI_STUDIO_PANE_TASKS, UMI_UI_LAYOUT_PANE, UMI_UI_HORIZONTAL, 1.0, 50 },
        { "bottom.terminal-history", "centre.bottom", UMI_STUDIO_PANE_TERMINAL_HISTORY, UMI_UI_LAYOUT_PANE, UMI_UI_HORIZONTAL, 1.0, 60 },
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
