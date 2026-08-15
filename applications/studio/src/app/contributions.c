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
#include "umicom/studio/workspace_profiles.h"

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
    { UMI_STUDIO_PANE_VCS_HISTORY, "Repository History", "studio.vcs-history", "document-open-recent-symbolic", UMI_UI_PLACEMENT_LEFT, 91, 0, 1, 1, { 420, 500 } },
    { UMI_STUDIO_PANE_VCS_BRANCHES, "Branches", "studio.vcs-branches", "view-list-tree-symbolic", UMI_UI_PLACEMENT_LEFT, 92, 0, 1, 1, { 360, 500 } },
    { UMI_STUDIO_PANE_VCS_REMOTES, "Remotes & Operations", "studio.vcs-remotes", "network-server-symbolic", UMI_UI_PLACEMENT_LEFT, 93, 0, 1, 1, { 440, 500 } },
    { UMI_STUDIO_PANE_VCS_DIFF, "Diff Viewer", "studio.vcs-diff", "document-properties-symbolic", UMI_UI_PLACEMENT_BOTTOM, 94, 0, 1, 1, { 760, 320 } },
    { UMI_STUDIO_PANE_RUN_DEBUG, "Run and Debug", "studio.run-debug", "system-run-symbolic", UMI_UI_PLACEMENT_LEFT, 100, 0, 1, 1, { 320, 500 } },
    { UMI_STUDIO_PANE_TESTING, "Testing", "studio.testing", "emblem-ok-symbolic", UMI_UI_PLACEMENT_LEFT, 110, 0, 1, 1, { 320, 500 } },
    { UMI_STUDIO_PANE_DESIGNER, "Designer", "studio.designer", "applications-graphics-symbolic", UMI_UI_PLACEMENT_LEFT, 120, 0, 1, 1, { 320, 500 } },
    { UMI_STUDIO_PANE_APPLICATIONS, "Applications", "studio.application-hub", "view-app-grid-symbolic", UMI_UI_PLACEMENT_LEFT, 130, 0, 1, 1, { 320, 500 } },
    { UMI_STUDIO_PANE_FRAMEWORK, "Framework", "studio.framework", "view-grid-symbolic", UMI_UI_PLACEMENT_LEFT, 140, 0, 1, 1, { 320, 500 } },
    { UMI_STUDIO_PANE_AI, "AI / AuthorEngine", "studio.authorengine", "mail-message-new-symbolic", UMI_UI_PLACEMENT_LEFT, 150, 0, 1, 1, { 320, 500 } },
    { UMI_STUDIO_PANE_AI_RUNTIMES, "AI Runtimes", "studio.ai-runtimes", "computer-symbolic", UMI_UI_PLACEMENT_LEFT, 151, 0, 1, 1, { 440, 500 } },
    { UMI_STUDIO_PANE_AI_CONTEXT, "AI Context", "studio.ai-context", "folder-documents-symbolic", UMI_UI_PLACEMENT_LEFT, 152, 0, 1, 1, { 440, 500 } },
    { UMI_STUDIO_PANE_AI_SESSIONS, "AI Sessions", "studio.ai-sessions", "document-open-recent-symbolic", UMI_UI_PLACEMENT_LEFT, 153, 0, 1, 1, { 440, 500 } },
    { UMI_STUDIO_PANE_AI_PRIVACY, "AI Privacy", "studio.ai-privacy", "security-high-symbolic", UMI_UI_PLACEMENT_LEFT, 154, 0, 1, 1, { 440, 500 } },
    { UMI_STUDIO_PANE_AI_CODING, "AI Coding Assistant", "studio.ai-coding", "applications-development-symbolic", UMI_UI_PLACEMENT_RIGHT, 155, 0, 1, 1, { 440, 560 } },
    { UMI_STUDIO_PANE_AI_CODING_CONTEXT, "AI Coding Context", "studio.ai-coding-context", "folder-documents-symbolic", UMI_UI_PLACEMENT_LEFT, 156, 0, 1, 1, { 480, 560 } },
    { UMI_STUDIO_PANE_AI_PATCH_REVIEW, "AI Patch Review", "studio.ai-patch-review", "document-properties-symbolic", UMI_UI_PLACEMENT_BOTTOM, 157, 0, 1, 1, { 820, 340 } },
    { UMI_STUDIO_PANE_EXTENSIONS, "Extensions", "studio.extensions-installed", "application-x-addon-symbolic", UMI_UI_PLACEMENT_LEFT, 160, 0, 1, 1, { 380, 500 } },
    { UMI_STUDIO_PANE_EXTENSION_CATALOGUE, "Extension Catalogue", "studio.extensions-catalogue", "system-search-symbolic", UMI_UI_PLACEMENT_LEFT, 161, 0, 1, 1, { 420, 500 } },
    { UMI_STUDIO_PANE_EXTENSION_PERMISSIONS, "Extension Permissions", "studio.extensions-permissions", "security-high-symbolic", UMI_UI_PLACEMENT_LEFT, 162, 0, 1, 1, { 420, 500 } },
    { UMI_STUDIO_PANE_EXTENSION_AUDIT, "Extension Audit", "studio.extensions-audit", "document-open-recent-symbolic", UMI_UI_PLACEMENT_BOTTOM, 163, 0, 1, 1, { 760, 280 } },
    { UMI_STUDIO_PANE_PRODUCT_MARKETPLACE, "Product Marketplace", "studio.product-marketplace", "system-software-install-symbolic", UMI_UI_PLACEMENT_LEFT, 170, 0, 1, 1, { 440, 500 } },
    { UMI_STUDIO_PANE_PRODUCTS_INSTALLED, "Installed Products", "studio.products-installed", "package-x-generic-symbolic", UMI_UI_PLACEMENT_LEFT, 171, 0, 1, 1, { 440, 500 } },
    { UMI_STUDIO_PANE_PRODUCT_UPDATES, "Product Updates", "studio.product-updates", "software-update-available-symbolic", UMI_UI_PLACEMENT_LEFT, 172, 0, 1, 1, { 440, 500 } },
    { UMI_STUDIO_PANE_PRODUCT_TRANSACTIONS, "Distribution Transactions", "studio.product-transactions", "document-open-recent-symbolic", UMI_UI_PLACEMENT_BOTTOM, 173, 0, 1, 1, { 760, 300 } },
    { UMI_STUDIO_PANE_PRODUCT_EVIDENCE, "Supply-chain Evidence", "studio.product-evidence", "security-high-symbolic", UMI_UI_PLACEMENT_BOTTOM, 174, 0, 1, 1, { 760, 300 } }
    ,{ UMI_STUDIO_PANE_KNOWLEDGE, "Knowledge Centre", "studio.knowledge", "folder-documents-symbolic", UMI_UI_PLACEMENT_LEFT, 180, 0, 1, 1, { 440, 540 } }
    ,{ UMI_STUDIO_PANE_KNOWLEDGE_COLLECTIONS, "Knowledge Collections", "studio.knowledge-collections", "view-list-tree-symbolic", UMI_UI_PLACEMENT_LEFT, 181, 0, 1, 1, { 440, 540 } }
    ,{ UMI_STUDIO_PANE_KNOWLEDGE_SOURCES, "Knowledge Sources", "studio.knowledge-sources", "folder-symbolic", UMI_UI_PLACEMENT_LEFT, 182, 0, 1, 1, { 500, 540 } }
    ,{ UMI_STUDIO_PANE_KNOWLEDGE_SEARCH, "Knowledge Search", "studio.knowledge-search", "system-search-symbolic", UMI_UI_PLACEMENT_RIGHT, 183, 0, 1, 1, { 560, 540 } }
    ,{ UMI_STUDIO_PANE_KNOWLEDGE_SOURCE, "Knowledge Source Viewer", "studio.knowledge-source", "text-x-generic-symbolic", UMI_UI_PLACEMENT_BOTTOM, 184, 0, 1, 1, { 820, 340 } }
};

static const UmiUiActionSnapshot STUDIO_ACTIONS[] = {
    { "studio.action.perspective.develop", UMI_STUDIO_COMMAND_PERSPECTIVE_ACTIVATE, "Develop", "Activate Develop perspective", "applications-development-symbolic", "Ctrl+1", 1, 1, 0, 0, 10, UMI_STUDIO_PERSPECTIVE_DEVELOP, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.perspective.build", UMI_STUDIO_COMMAND_PERSPECTIVE_ACTIVATE, "Build", "Activate Build perspective", "system-run-symbolic", "Ctrl+2", 1, 1, 0, 0, 20, UMI_STUDIO_PERSPECTIVE_BUILD, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.perspective.test", UMI_STUDIO_COMMAND_PERSPECTIVE_ACTIVATE, "Test", "Activate Test perspective", "emblem-ok-symbolic", "Ctrl+3", 1, 1, 0, 0, 30, UMI_STUDIO_PERSPECTIVE_TEST, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.explorer", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Project Explorer", "Show or hide Project Explorer", "folder-symbolic", "Ctrl+Shift+E", 1, 1, 1, 1, 40, UMI_STUDIO_PANE_EXPLORER, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.output", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Output", "Show or hide Output", "utilities-terminal-symbolic", "Ctrl+J", 1, 1, 1, 1, 50, UMI_STUDIO_PANE_OUTPUT, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.problems", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Problems", "Show or hide Problems", "dialog-warning-symbolic", "Ctrl+Shift+M", 1, 1, 1, 1, 60, UMI_STUDIO_PANE_PROBLEMS, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.layout.reset", UMI_STUDIO_COMMAND_LAYOUT_RESET, "Reset Layout", "Restore the default Studio layout", "view-restore-symbolic", "", 1, 1, 0, 0, 70, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.workspace-profile.develop", UMI_STUDIO_COMMAND_WORKSPACE_PROFILE_ACTIVATE, "Develop Layout", "Editor-first coding workspace", "applications-development-symbolic", "", 1, 1, 0, 0, 71, UMI_STUDIO_WORKSPACE_PROFILE_DEVELOP, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.workspace-profile.focus", UMI_STUDIO_COMMAND_WORKSPACE_PROFILE_ACTIVATE, "Focus Layout", "Hide tool regions for distraction-free editing", "view-fullscreen-symbolic", "Ctrl+Shift+F12", 1, 1, 0, 0, 72, UMI_STUDIO_WORKSPACE_PROFILE_FOCUS, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.workspace-profile.debug", UMI_STUDIO_COMMAND_WORKSPACE_PROFILE_ACTIVATE, "Debug Layout", "Show source, execution state and debugging tools", "applications-engineering-symbolic", "", 1, 1, 0, 0, 73, UMI_STUDIO_WORKSPACE_PROFILE_DEBUG, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.workspace-profile.review", UMI_STUDIO_COMMAND_WORKSPACE_PROFILE_ACTIVATE, "Review Layout", "Show comparison and review context", "document-properties-symbolic", "", 1, 1, 0, 0, 74, UMI_STUDIO_WORKSPACE_PROFILE_REVIEW, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.workspace-profile.operations", UMI_STUDIO_COMMAND_WORKSPACE_PROFILE_ACTIVATE, "Operations Layout", "Show monitoring and operational tools", "utilities-system-monitor-symbolic", "", 1, 1, 0, 0, 75, UMI_STUDIO_WORKSPACE_PROFILE_OPERATIONS, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.workspace-profile.trading", UMI_STUDIO_COMMAND_WORKSPACE_PROFILE_ACTIVATE, "Trading Layout", "Show market, analytics, order and risk tools", "view-statistics-symbolic", "", 1, 1, 0, 0, 76, UMI_STUDIO_WORKSPACE_PROFILE_TRADING, UMI_UI_ACTION_ARGUMENT_NONE },
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
    { "studio.action.view.command-palette", UMI_STUDIO_COMMAND_QUICK_ACCESS_SHOW, "Command Palette…", "Search and execute registered Studio actions and Framework commands", "system-search-symbolic", "Ctrl+Shift+P", 1, 1, 0, 0, 180, "", UMI_UI_ACTION_ARGUMENT_NONE },
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
    { "studio.action.debug.add-breakpoint", UMI_STUDIO_COMMAND_DEBUG_ADD_BREAKPOINT, "Add Breakpoint…", "Add a Framework-owned source breakpoint using path:line", "media-record-symbolic", "F9", 1, 1, 0, 0, 390, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.vcs.refresh", UMI_STUDIO_COMMAND_VCS_REFRESH, "Refresh Source Control", "Refresh working tree, history, branches, remotes and tags", "view-refresh-symbolic", "Ctrl+Shift+G", 1, 1, 0, 0, 400, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.vcs.stage", UMI_STUDIO_COMMAND_VCS_STAGE, "Stage Path…", "Stage a repository-relative path", "list-add-symbolic", "", 1, 1, 0, 0, 410, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.vcs.unstage", UMI_STUDIO_COMMAND_VCS_UNSTAGE, "Unstage Path…", "Remove a path from the index", "list-remove-symbolic", "", 1, 1, 0, 0, 420, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.vcs.stage-all", UMI_STUDIO_COMMAND_VCS_STAGE_ALL, "Stage All", "Stage every working-tree change", "list-add-symbolic", "", 1, 1, 0, 0, 430, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.vcs.unstage-all", UMI_STUDIO_COMMAND_VCS_UNSTAGE_ALL, "Unstage All", "Remove every indexed change", "list-remove-symbolic", "", 1, 1, 0, 0, 440, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.vcs.discard", UMI_STUDIO_COMMAND_VCS_DISCARD, "Discard Path…", "Discard a working-tree path", "edit-delete-symbolic", "", 1, 1, 0, 0, 450, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.vcs.commit", UMI_STUDIO_COMMAND_VCS_COMMIT, "Commit…", "Create a commit from indexed changes", "document-save-symbolic", "Ctrl+Enter", 1, 1, 0, 0, 460, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.vcs.fetch", UMI_STUDIO_COMMAND_VCS_FETCH, "Fetch", "Fetch all remotes and prune stale refs", "folder-download-symbolic", "", 1, 1, 0, 0, 470, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.vcs.pull", UMI_STUDIO_COMMAND_VCS_PULL, "Pull", "Fast-forward the active branch", "go-down-symbolic", "", 1, 1, 0, 0, 480, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.vcs.push", UMI_STUDIO_COMMAND_VCS_PUSH, "Push", "Push the active branch", "go-up-symbolic", "", 1, 1, 0, 0, 490, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.vcs.branch-create", UMI_STUDIO_COMMAND_VCS_BRANCH_CREATE, "New Branch…", "Create and check out a branch", "list-add-symbolic", "", 1, 1, 0, 0, 500, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.vcs.branch-checkout", UMI_STUDIO_COMMAND_VCS_BRANCH_CHECKOUT, "Checkout Branch…", "Switch to an existing branch", "go-jump-symbolic", "", 1, 1, 0, 0, 510, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.vcs.branch-delete", UMI_STUDIO_COMMAND_VCS_BRANCH_DELETE, "Delete Branch…", "Safely delete a merged branch", "edit-delete-symbolic", "", 1, 1, 0, 0, 520, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.vcs.diff", UMI_STUDIO_COMMAND_VCS_DIFF, "Open Diff…", "Open a working-tree or staged path diff", "document-properties-symbolic", "", 1, 1, 0, 0, 530, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.extensions.refresh", UMI_STUDIO_COMMAND_PLUGINS_REPORT, "Refresh Extensions", "Refresh and report installed extension state", "view-refresh-symbolic", "", 1, 1, 0, 0, 540, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.extension-catalogue", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Extension Catalogue", "Show or hide the Extension Catalogue", "system-search-symbolic", "", 1, 1, 1, 1, 550, UMI_STUDIO_PANE_EXTENSION_CATALOGUE, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.extension-permissions", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Extension Permissions", "Show or hide extension permissions", "security-high-symbolic", "", 1, 1, 1, 1, 560, UMI_STUDIO_PANE_EXTENSION_PERMISSIONS, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.extension-audit", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Extension Audit", "Show or hide extension audit history", "document-open-recent-symbolic", "", 1, 1, 1, 1, 570, UMI_STUDIO_PANE_EXTENSION_AUDIT, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.vcs-history", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Repository History", "Show repository history", "document-open-recent-symbolic", "", 1, 1, 1, 1, 540, UMI_STUDIO_PANE_VCS_HISTORY, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.vcs-branches", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Branches", "Show repository branches", "view-list-tree-symbolic", "", 1, 1, 1, 1, 550, UMI_STUDIO_PANE_VCS_BRANCHES, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.vcs-remotes", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Remotes & Operations", "Show remotes and operation journal", "network-server-symbolic", "", 1, 1, 1, 1, 560, UMI_STUDIO_PANE_VCS_REMOTES, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.vcs-diff", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Diff Viewer", "Show source-control diff viewer", "document-properties-symbolic", "", 1, 1, 1, 1, 570, UMI_STUDIO_PANE_VCS_DIFF, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.ai.refresh-health", UMI_STUDIO_COMMAND_AI_REFRESH_HEALTH, "Refresh AI Health", "Probe AI providers and AuthorEngine runtimes", "view-refresh-symbolic", "", 1, 1, 0, 0, 572, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.ai.new-session", UMI_STUDIO_COMMAND_AI_NEW_SESSION, "New AI Session…", "Create a governed AuthorEngine conversation session", "document-new-symbolic", "", 1, 1, 0, 0, 574, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.ai.save-session", UMI_STUDIO_COMMAND_AI_SAVE_SESSION, "Save AI Session…", "Persist the active AI session when privacy policy permits", "document-save-symbolic", "", 1, 1, 0, 0, 576, "", UMI_UI_ACTION_ARGUMENT_SAVE_PATH },
    { "studio.action.pane.ai-workspace", UMI_STUDIO_COMMAND_PANE_TOGGLE, "AI Workspace", "Open the AuthorEngine integration overview", "mail-message-new-symbolic", "", 1, 1, 1, 1, 578, UMI_STUDIO_PANE_AI, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.ai-runtimes", UMI_STUDIO_COMMAND_PANE_TOGGLE, "AI Runtimes", "Inspect provider and model runtimes", "computer-symbolic", "", 1, 1, 1, 1, 580, UMI_STUDIO_PANE_AI_RUNTIMES, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.ai-context", UMI_STUDIO_COMMAND_PANE_TOGGLE, "AI Context", "Inspect context sources and token budgets", "folder-documents-symbolic", "", 1, 1, 1, 1, 582, UMI_STUDIO_PANE_AI_CONTEXT, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.ai-sessions", UMI_STUDIO_COMMAND_PANE_TOGGLE, "AI Sessions", "Inspect conversation session state", "document-open-recent-symbolic", "", 1, 1, 1, 1, 584, UMI_STUDIO_PANE_AI_SESSIONS, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.ai-privacy", UMI_STUDIO_COMMAND_PANE_TOGGLE, "AI Privacy", "Inspect remote sharing, approval and persistence controls", "security-high-symbolic", "", 1, 1, 1, 1, 586, UMI_STUDIO_PANE_AI_PRIVACY, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.ai.code-chat", UMI_STUDIO_COMMAND_AI_CODE_CHAT, "Code Chat…", "Discuss the active code with governed repository context", "mail-message-new-symbolic", "Ctrl+Alt+A", 1, 1, 0, 0, 588, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.ai.complete-code", UMI_STUDIO_COMMAND_AI_COMPLETE_CODE, "Complete Code…", "Plan context-aware completion", "insert-text-symbolic", "", 1, 1, 0, 0, 590, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.ai.explain-code", UMI_STUDIO_COMMAND_AI_EXPLAIN_CODE, "Explain Code…", "Explain code with repository context", "dialog-information-symbolic", "", 1, 1, 0, 0, 592, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.ai.refactor-code", UMI_STUDIO_COMMAND_AI_REFACTOR_CODE, "Refactor Code…", "Prepare a reviewable refactoring patch", "applications-development-symbolic", "", 1, 1, 0, 0, 594, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.ai.generate-tests", UMI_STUDIO_COMMAND_AI_GENERATE_TESTS, "Generate Tests…", "Prepare reviewable repository-aware tests", "emblem-ok-symbolic", "", 1, 1, 0, 0, 596, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.ai.patch-approve", UMI_STUDIO_COMMAND_AI_PATCH_APPROVE, "Approve AI Patch…", "Approve the exact current patch preview", "emblem-ok-symbolic", "", 1, 1, 0, 0, 598, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.ai.patch-apply", UMI_STUDIO_COMMAND_AI_PATCH_APPLY, "Apply AI Patch…", "Apply an approved conflict-free patch", "document-save-symbolic", "", 1, 1, 0, 0, 600, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.ai.patch-revert", UMI_STUDIO_COMMAND_AI_PATCH_REVERT, "Revert AI Patch…", "Revert an unchanged applied patch", "edit-undo-symbolic", "", 1, 1, 0, 0, 602, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.pane.ai-coding", UMI_STUDIO_COMMAND_PANE_TOGGLE, "AI Coding Assistant", "Open AI coding workflows", "applications-development-symbolic", "", 1, 1, 1, 1, 604, UMI_STUDIO_PANE_AI_CODING, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.ai-coding-context", UMI_STUDIO_COMMAND_PANE_TOGGLE, "AI Coding Context", "Inspect repository prompt context", "folder-documents-symbolic", "", 1, 1, 1, 1, 606, UMI_STUDIO_PANE_AI_CODING_CONTEXT, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.ai-patch-review", UMI_STUDIO_COMMAND_PANE_TOGGLE, "AI Patch Review", "Inspect and control the latest coding patch", "document-properties-symbolic", "", 1, 1, 1, 1, 608, UMI_STUDIO_PANE_AI_PATCH_REVIEW, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.knowledge.collection-create", UMI_STUDIO_COMMAND_KNOWLEDGE_COLLECTION_CREATE, "New Knowledge Collection…", "Create an isolated knowledge collection", "document-new-symbolic", "", 1, 1, 0, 0, 610, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.knowledge.ingest", UMI_STUDIO_COMMAND_KNOWLEDGE_INGEST, "Add Knowledge Source…", "Ingest text with source provenance", "document-open-symbolic", "", 1, 1, 0, 0, 612, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.knowledge.refresh", UMI_STUDIO_COMMAND_KNOWLEDGE_REFRESH, "Refresh Knowledge", "Refresh changed knowledge sources", "view-refresh-symbolic", "", 1, 1, 0, 0, 614, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.knowledge.search", UMI_STUDIO_COMMAND_KNOWLEDGE_SEARCH, "Search Knowledge…", "Search indexed project, code and document evidence", "system-search-symbolic", "Ctrl+Alt+K", 1, 1, 0, 0, 616, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.pane.knowledge", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Knowledge Centre", "Open the Native RAG overview", "folder-documents-symbolic", "", 1, 1, 1, 1, 618, UMI_STUDIO_PANE_KNOWLEDGE, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.knowledge-collections", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Knowledge Collections", "Inspect retrieval collections", "view-list-tree-symbolic", "", 1, 1, 1, 1, 620, UMI_STUDIO_PANE_KNOWLEDGE_COLLECTIONS, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.knowledge-sources", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Knowledge Sources", "Inspect indexed sources", "folder-symbolic", "", 1, 1, 1, 1, 622, UMI_STUDIO_PANE_KNOWLEDGE_SOURCES, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.knowledge-search", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Knowledge Search", "Open cited search results", "system-search-symbolic", "", 1, 1, 1, 1, 624, UMI_STUDIO_PANE_KNOWLEDGE_SEARCH, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.knowledge-source", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Knowledge Source Viewer", "Open the exact cited source range", "text-x-generic-symbolic", "", 1, 1, 1, 1, 626, UMI_STUDIO_PANE_KNOWLEDGE_SOURCE, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.marketplace.check-updates", UMI_STUDIO_COMMAND_MARKETPLACE_CHECK_UPDATES, "Check for Product Updates", "Evaluate installed products against verified releases", "view-refresh-symbolic", "", 1, 1, 0, 0, 580, "", UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.marketplace.plan-update", UMI_STUDIO_COMMAND_MARKETPLACE_PLAN_UPDATE, "Plan Product Update…", "Create a recoverable update plan for a release ID", "system-software-install-symbolic", "", 1, 1, 0, 0, 590, "", UMI_UI_ACTION_ARGUMENT_TEXT },
    { "studio.action.pane.product-marketplace", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Product Marketplace", "Browse verified application, runtime and template releases", "system-software-install-symbolic", "", 1, 1, 1, 1, 600, UMI_STUDIO_PANE_PRODUCT_MARKETPLACE, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.products-installed", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Installed Products", "Inspect installed versions and rollback readiness", "package-x-generic-symbolic", "", 1, 1, 1, 1, 610, UMI_STUDIO_PANE_PRODUCTS_INSTALLED, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.product-updates", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Product Updates", "Inspect verified update notifications", "software-update-available-symbolic", "", 1, 1, 1, 1, 620, UMI_STUDIO_PANE_PRODUCT_UPDATES, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.product-transactions", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Distribution Transactions", "Inspect install, update and rollback plans", "document-open-recent-symbolic", "", 1, 1, 1, 1, 630, UMI_STUDIO_PANE_PRODUCT_TRANSACTIONS, UMI_UI_ACTION_ARGUMENT_NONE },
    { "studio.action.pane.product-evidence", UMI_STUDIO_COMMAND_PANE_TOGGLE, "Supply-chain Evidence", "Inspect checksum, signature, SBOM, licence and provenance evidence", "security-high-symbolic", "", 1, 1, 1, 1, 640, UMI_STUDIO_PANE_PRODUCT_EVIDENCE, UMI_UI_ACTION_ARGUMENT_NONE }
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
    { "menu.window.develop", "window", "workspace-profiles", "studio.action.workspace-profile.develop", "", 0, 10 },
    { "menu.window.focus", "window", "workspace-profiles", "studio.action.workspace-profile.focus", "", 0, 20 },
    { "menu.window.debug", "window", "workspace-profiles", "studio.action.workspace-profile.debug", "", 0, 30 },
    { "menu.window.review", "window", "workspace-profiles", "studio.action.workspace-profile.review", "", 0, 40 },
    { "menu.window.operations", "window", "workspace-profiles", "studio.action.workspace-profile.operations", "", 0, 50 },
    { "menu.window.trading", "window", "workspace-profiles", "studio.action.workspace-profile.trading", "", 0, 60 },
    { "menu.window.separator", "window", "workspace-profiles", "", "", 1, 70 },
    { "menu.window.reset", "window", "workspace-profiles", "studio.action.layout.reset", "", 0, 80 },
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
    { "menu.source-control.refresh", "source-control", "repository", "studio.action.vcs.refresh", "", 0, 10 },
    { "menu.source-control.stage-all", "source-control", "changes", "studio.action.vcs.stage-all", "", 0, 20 },
    { "menu.source-control.unstage-all", "source-control", "changes", "studio.action.vcs.unstage-all", "", 0, 30 },
    { "menu.source-control.commit", "source-control", "changes", "studio.action.vcs.commit", "", 0, 40 },
    { "menu.source-control.fetch", "source-control", "remote", "studio.action.vcs.fetch", "", 0, 50 },
    { "menu.source-control.pull", "source-control", "remote", "studio.action.vcs.pull", "", 0, 60 },
    { "menu.source-control.push", "source-control", "remote", "studio.action.vcs.push", "", 0, 70 },
    { "menu.source-control.history", "source-control", "views", "studio.action.pane.vcs-history", "", 0, 80 },
    { "menu.source-control.branches", "source-control", "views", "studio.action.pane.vcs-branches", "", 0, 90 },
    { "menu.source-control.remotes", "source-control", "views", "studio.action.pane.vcs-remotes", "", 0, 100 },
    { "menu.source-control.diff", "source-control", "views", "studio.action.pane.vcs-diff", "", 0, 110 },
    { "menu.extensions.refresh", "extensions", "management", "studio.action.extensions.refresh", "", 0, 10 },
    { "menu.extensions.catalogue", "extensions", "views", "studio.action.pane.extension-catalogue", "", 0, 20 },
    { "menu.extensions.permissions", "extensions", "views", "studio.action.pane.extension-permissions", "", 0, 30 },
    { "menu.extensions.audit", "extensions", "views", "studio.action.pane.extension-audit", "", 0, 40 },
    { "menu.ai.refresh", "ai", "management", "studio.action.ai.refresh-health", "", 0, 10 },
    { "menu.ai.new-session", "ai", "sessions", "studio.action.ai.new-session", "", 0, 20 },
    { "menu.ai.save-session", "ai", "sessions", "studio.action.ai.save-session", "", 0, 30 },
    { "menu.ai.workspace", "ai", "views", "studio.action.pane.ai-workspace", "", 0, 40 },
    { "menu.ai.runtimes", "ai", "views", "studio.action.pane.ai-runtimes", "", 0, 50 },
    { "menu.ai.context", "ai", "views", "studio.action.pane.ai-context", "", 0, 60 },
    { "menu.ai.sessions", "ai", "views", "studio.action.pane.ai-sessions", "", 0, 70 },
    { "menu.ai.privacy", "ai", "views", "studio.action.pane.ai-privacy", "", 0, 80 },
    { "menu.products.check-updates", "products", "management", "studio.action.marketplace.check-updates", "", 0, 10 },
    { "menu.products.plan-update", "products", "management", "studio.action.marketplace.plan-update", "", 0, 20 },
    { "menu.products.marketplace", "products", "views", "studio.action.pane.product-marketplace", "", 0, 30 },
    { "menu.products.installed", "products", "views", "studio.action.pane.products-installed", "", 0, 40 },
    { "menu.products.updates", "products", "views", "studio.action.pane.product-updates", "", 0, 50 },
    { "menu.products.transactions", "products", "views", "studio.action.pane.product-transactions", "", 0, 60 },
    { "menu.products.evidence", "products", "views", "studio.action.pane.product-evidence", "", 0, 70 },
    { "menu.help.about", "help", "about", "studio.action.notification.info", "", 0, 10 }
};

static const UmiUiToolbarSnapshot STUDIO_TOOLBAR[] = {
    { "toolbar.develop", "main", "studio.action.perspective.develop", "perspectives", 0, 10 },
    { "toolbar.build", "main", "studio.action.perspective.build", "perspectives", 0, 20 },
    { "toolbar.test", "main", "studio.action.perspective.test", "perspectives", 0, 30 },
    { "toolbar.separator", "main", "", "layout", 1, 40 },
    { "toolbar.explorer", "main", "studio.action.pane.explorer", "layout", 0, 50 },
    { "toolbar.output", "main", "studio.action.pane.output", "layout", 0, 60 },
    { "toolbar.layout-reset", "main", "studio.action.layout.reset", "layout", 0, 65 },
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
    { "studio.contribution.vcs-history", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_VCS_HISTORY, 91, 1 },
    { "studio.contribution.vcs-branches", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_VCS_BRANCHES, 92, 1 },
    { "studio.contribution.vcs-remotes", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_VCS_REMOTES, 93, 1 },
    { "studio.contribution.vcs-diff", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_VCS_DIFF, 94, 1 },
    { "studio.contribution.run-debug", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_RUN_DEBUG, 100, 1 },
    { "studio.contribution.testing", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_TESTING, 110, 1 },
    { "studio.contribution.designer", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_DESIGNER, 120, 1 },
    { "studio.contribution.applications", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_APPLICATIONS, 130, 1 },
    { "studio.contribution.framework", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_FRAMEWORK, 140, 1 },
    { "studio.contribution.ai", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_AI, 150, 1 },
    { "studio.contribution.ai-runtimes", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_AI_RUNTIMES, 151, 1 },
    { "studio.contribution.ai-context", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_AI_CONTEXT, 152, 1 },
    { "studio.contribution.ai-sessions", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_AI_SESSIONS, 153, 1 },
    { "studio.contribution.ai-privacy", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_AI_PRIVACY, 154, 1 },
    { "studio.contribution.ai-coding", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_AI_CODING, 155, 1 },
    { "studio.contribution.ai-coding-context", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_AI_CODING_CONTEXT, 156, 1 },
    { "studio.contribution.ai-patch-review", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_AI_PATCH_REVIEW, 157, 1 },
    { "studio.contribution.extensions", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_EXTENSIONS, 160, 1 },
    { "studio.contribution.extension-catalogue", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_EXTENSION_CATALOGUE, 161, 1 },
    { "studio.contribution.extension-permissions", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_EXTENSION_PERMISSIONS, 162, 1 },
    { "studio.contribution.extension-audit", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_EXTENSION_AUDIT, 163, 1 },
    { "studio.contribution.product-marketplace", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_PRODUCT_MARKETPLACE, 170, 1 },
    { "studio.contribution.products-installed", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_PRODUCTS_INSTALLED, 171, 1 },
    { "studio.contribution.product-updates", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_PRODUCT_UPDATES, 172, 1 },
    { "studio.contribution.product-transactions", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_PRODUCT_TRANSACTIONS, 173, 1 },
    { "studio.contribution.product-evidence", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_PRODUCT_EVIDENCE, 174, 1 }
    ,{ "studio.contribution.knowledge", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_KNOWLEDGE, 180, 1 }
    ,{ "studio.contribution.knowledge-collections", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_KNOWLEDGE_COLLECTIONS, 181, 1 }
    ,{ "studio.contribution.knowledge-sources", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_KNOWLEDGE_SOURCES, 182, 1 }
    ,{ "studio.contribution.knowledge-search", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_KNOWLEDGE_SEARCH, 183, 1 }
    ,{ "studio.contribution.knowledge-source", "org.umicom.studio.shell", "umicom.ui.panes", UMI_STUDIO_PANE_KNOWLEDGE_SOURCE, 184, 1 }
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
