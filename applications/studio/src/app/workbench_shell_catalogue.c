/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/workbench_shell_catalogue.c
 *
 * PURPOSE:
 *   Compose Studio's Activity Bar, view containers, context keys, keybindings, breadcrumb and Explorer seed from reusable Framework contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file contains Studio-specific IDs and labels only. The models, search, conflict detection, state transitions and GTK rendering are Framework capabilities.
 */

#include "umicom/studio/workbench_shell_catalogue.h"

#include <stdio.h>
#include <string.h>

#include "umicom/studio/contributions.h"
#include "umicom/studio/workbench_commands.h"

typedef struct StudioActivityDefinition {
    const char *activity_id;
    const char *container_id;
    const char *label;
    const char *icon_name;
    int32_t order;
} StudioActivityDefinition;

static const StudioActivityDefinition ACTIVITIES[] = {
    { UMI_STUDIO_ACTIVITY_EXPLORER, UMI_STUDIO_CONTAINER_EXPLORER,
      "Explorer", "folder-symbolic", 10 },
    { UMI_STUDIO_ACTIVITY_SEARCH, UMI_STUDIO_CONTAINER_SEARCH,
      "Search", "system-search-symbolic", 20 },
    { UMI_STUDIO_ACTIVITY_SCM, UMI_STUDIO_CONTAINER_SCM,
      "Source Control", "org.gnome.Builder-vcs-symbolic", 30 },
    { UMI_STUDIO_ACTIVITY_RUN, UMI_STUDIO_CONTAINER_RUN,
      "Run and Debug", "system-run-symbolic", 40 },
    { UMI_STUDIO_ACTIVITY_TEST, UMI_STUDIO_CONTAINER_TEST,
      "Testing", "emblem-ok-symbolic", 50 },
    { UMI_STUDIO_ACTIVITY_DESIGNER, UMI_STUDIO_CONTAINER_DESIGNER,
      "Designer", "applications-graphics-symbolic", 60 },
    { UMI_STUDIO_ACTIVITY_APPLICATIONS, UMI_STUDIO_CONTAINER_APPLICATIONS,
      "Applications", "view-app-grid-symbolic", 70 },
    { UMI_STUDIO_ACTIVITY_FRAMEWORK, UMI_STUDIO_CONTAINER_FRAMEWORK,
      "Framework", "view-grid-symbolic", 80 },
    { UMI_STUDIO_ACTIVITY_AI, UMI_STUDIO_CONTAINER_AI,
      "AI / AuthorEngine", "mail-message-new-symbolic", 90 }
};

static UmiStatus register_container(UmiUiWorkbench *workbench,
                                    const char *id,
                                    const char *title,
                                    const char *description,
                                    const char *icon_name,
                                    int32_t order,
                                    const char *const *views,
                                    size_t view_count)
{
    UmiUiViewContainerSnapshot item = {0};
    size_t index;

    (void)snprintf(item.container_id, sizeof(item.container_id), "%s", id);
    (void)snprintf(item.title, sizeof(item.title), "%s", title);
    (void)snprintf(item.description, sizeof(item.description), "%s", description);
    (void)snprintf(item.icon_name, sizeof(item.icon_name), "%s", icon_name);
    item.placement = UMI_UI_PLACEMENT_LEFT;
    item.order = order;
    item.visible = 1;
    item.view_count = view_count;

    if (view_count > UMI_UI_VIEW_CONTAINER_MAX_VIEWS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U; index < view_count; ++index) {
        (void)snprintf(item.view_ids[index], sizeof(item.view_ids[index]),
                       "%s", views[index]);
    }
    if (view_count > 0U) {
        (void)snprintf(item.active_view_id, sizeof(item.active_view_id),
                       "%s", views[0]);
    }
    return umi_ui_view_container_model_upsert(
        umi_ui_workbench_view_containers(workbench), &item);
}

static UmiStatus register_view_containers(UmiUiWorkbench *workbench)
{
    static const char *const EXPLORER[] = {
        UMI_STUDIO_PANE_EXPLORER
    };
    static const char *const SEARCH[] = {
        UMI_STUDIO_PANE_SEARCH
    };
    static const char *const SCM[] = {
        UMI_STUDIO_PANE_SOURCE_CONTROL,
        UMI_STUDIO_PANE_VCS_COMMIT,
        UMI_STUDIO_PANE_VCS_HISTORY,
        UMI_STUDIO_PANE_VCS_BRANCHES,
        UMI_STUDIO_PANE_VCS_REMOTES,
        UMI_STUDIO_PANE_VCS_CONFLICTS,
        UMI_STUDIO_PANE_VCS_DIFF,
        UMI_STUDIO_PANE_VCS_OPERATIONS
    };
    static const char *const RUN[] = {
        UMI_STUDIO_PANE_RUN_DEBUG,
        UMI_STUDIO_PANE_DEBUG_CALL_STACK,
        UMI_STUDIO_PANE_DEBUG_VARIABLES,
        UMI_STUDIO_PANE_DEBUG_WATCHES,
        UMI_STUDIO_PANE_DEBUG_BREAKPOINTS,
        UMI_STUDIO_PANE_DEBUG_CONSOLE,
        UMI_STUDIO_PANE_OUTPUT,
        UMI_STUDIO_PANE_TERMINAL
    };
    static const char *const TEST[] = {
        UMI_STUDIO_PANE_TESTING,
        UMI_STUDIO_PANE_PROBLEMS,
        UMI_STUDIO_PANE_OUTPUT
    };
    static const char *const DESIGNER[] = {
        UMI_STUDIO_PANE_DESIGNER,
        "studio.designer.palette",
        "studio.designer.tree",
        "studio.designer.inspector"
    };
    static const char *const APPLICATIONS[] = {
        UMI_STUDIO_PANE_APPLICATIONS,
        UMI_STUDIO_PANE_APPLICATION_COMPONENTS,
        UMI_STUDIO_PANE_GTK4_COVERAGE
    };
    static const char *const FRAMEWORK[] = {
        UMI_STUDIO_PANE_FRAMEWORK,
        UMI_STUDIO_PANE_ARCHITECTURE
    };
    static const char *const AI[] = {
        UMI_STUDIO_PANE_AI,
        UMI_STUDIO_PANE_CHAT,
        "studio.authorengine"
    };
    UmiStatus status;

#define REGISTER_CONTAINER(id, title, desc, icon, order, views) \
    do { \
        status = register_container(workbench, id, title, desc, icon, order, \
                                    views, sizeof(views) / sizeof((views)[0])); \
        if (status != UMI_STATUS_OK) return status; \
    } while (0)

    REGISTER_CONTAINER(UMI_STUDIO_CONTAINER_EXPLORER,
        "Explorer", "Workspace files, open editors and project navigation",
        "folder-symbolic", 10, EXPLORER);
    REGISTER_CONTAINER(UMI_STUDIO_CONTAINER_SEARCH,
        "Search", "Workspace text, symbols and indexed resources",
        "system-search-symbolic", 20, SEARCH);
    REGISTER_CONTAINER(UMI_STUDIO_CONTAINER_SCM,
        "Source Control", "Changes, commits, history, branches, remotes, conflicts, diffs and operations",
        "org.gnome.Builder-vcs-symbolic", 30, SCM);
    REGISTER_CONTAINER(UMI_STUDIO_CONTAINER_RUN,
        "Run and Debug", "Build output, run configurations, debugger and terminal",
        "system-run-symbolic", 40, RUN);
    REGISTER_CONTAINER(UMI_STUDIO_CONTAINER_TEST,
        "Testing", "Test discovery, results, failures and coverage evidence",
        "emblem-ok-symbolic", 50, TEST);
    REGISTER_CONTAINER(UMI_STUDIO_CONTAINER_DESIGNER,
        "Designer", "Framework-backed visual application designer",
        "applications-graphics-symbolic", 60, DESIGNER);
    REGISTER_CONTAINER(UMI_STUDIO_CONTAINER_APPLICATIONS,
        "Applications", "Application Hub, suites and independent Umicom products",
        "view-app-grid-symbolic", 70, APPLICATIONS);
    REGISTER_CONTAINER(UMI_STUDIO_CONTAINER_FRAMEWORK,
        "Framework", "Framework capabilities, modules, architecture and diagnostics",
        "view-grid-symbolic", 80, FRAMEWORK);
    REGISTER_CONTAINER(UMI_STUDIO_CONTAINER_AI,
        "AI / AuthorEngine", "AI chat, retrieval, AuthorEngine and governed Helix tools",
        "mail-message-new-symbolic", 90, AI);

#undef REGISTER_CONTAINER
    return UMI_STATUS_OK;
}

static UmiStatus register_activities(UmiUiWorkbench *workbench)
{
    size_t index;
    UmiStatus status;
    for (index = 0U; index < sizeof(ACTIVITIES) / sizeof(ACTIVITIES[0]); ++index) {
        UmiUiActivitySnapshot activity = {0};
        (void)snprintf(activity.activity_id, sizeof(activity.activity_id),
                       "%s", ACTIVITIES[index].activity_id);
        (void)snprintf(activity.container_id, sizeof(activity.container_id),
                       "%s", ACTIVITIES[index].container_id);
        (void)snprintf(activity.label, sizeof(activity.label),
                       "%s", ACTIVITIES[index].label);
        (void)snprintf(activity.icon_name, sizeof(activity.icon_name),
                       "%s", ACTIVITIES[index].icon_name);
        activity.order = ACTIVITIES[index].order;
        activity.visible = 1;
        activity.enabled = 1;
        activity.active = index == 0U;
        status = umi_ui_activity_model_upsert(
            umi_ui_workbench_activities(workbench), &activity);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

static UmiStatus register_context(UmiUiWorkbench *workbench)
{
    UmiUiContextStore *context = umi_ui_workbench_context(workbench);
    UmiStatus status;

    status = umi_ui_context_set_boolean(context, "studio.workspace.open", 0);
    if (status == UMI_STATUS_OK) {
        status = umi_ui_context_set_string(context, "studio.mode", "develop");
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_context_set_boolean(context, "studio.ui.sidebar.visible", 1);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_context_set_boolean(context, "studio.ui.bottom.visible", 1);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_context_set_boolean(context, "studio.ui.auxiliary.visible", 0);
    }
    if (status == UMI_STATUS_OK) {
        /*
         * Presentation adapters consume semantic preferences from the shared
         * context store. This keeps theme and density choices toolkit-neutral
         * and leaves room for persisted user settings in a later batch.
         */
        status = umi_ui_context_set_string(context,
                                           "studio.ui.theme",
                                           "umicom-dark");
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_context_set_string(context,
                                           "studio.ui.density",
                                           "compact");
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_context_set_boolean(context, "studio.designer.available", 1);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_context_set_boolean(context, "studio.applications.available", 1);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_context_set_boolean(context, "studio.ai.available", 1);
    }
    return status;
}

static UmiStatus register_keybinding(UmiUiWorkbench *workbench,
                                     const char *id,
                                     const char *command,
                                     const char *argument,
                                     const char *chord,
                                     const char *when_expression,
                                     int32_t order)
{
    UmiUiKeybindingSnapshot item = {0};
    (void)snprintf(item.binding_id, sizeof(item.binding_id), "%s", id);
    (void)snprintf(item.command_id, sizeof(item.command_id), "%s", command);
    (void)snprintf(item.argument, sizeof(item.argument), "%s",
                   argument != NULL ? argument : "");
    (void)snprintf(item.chord, sizeof(item.chord), "%s", chord);
    (void)snprintf(item.when_expression, sizeof(item.when_expression), "%s",
                   when_expression != NULL ? when_expression : "");
    item.order = order;
    item.enabled = 1;
    return umi_ui_keybinding_registry_upsert(
        umi_ui_workbench_keybindings(workbench), &item);
}

static UmiStatus register_keybindings(UmiUiWorkbench *workbench)
{
    static const struct {
        const char *id;
        const char *argument;
        const char *chord;
        const char *when_expression;
        int32_t order;
    } ACTIVATION_KEYS[] = {
        { "studio.keys.explorer", UMI_STUDIO_ACTIVITY_EXPLORER,
          "Ctrl+Shift+E", "", 10 },
        { "studio.keys.search", UMI_STUDIO_ACTIVITY_SEARCH,
          "Ctrl+Shift+F", "", 20 },
        { "studio.keys.scm", UMI_STUDIO_ACTIVITY_SCM,
          "Ctrl+Shift+G", "", 30 },
        { "studio.keys.run", UMI_STUDIO_ACTIVITY_RUN,
          "Ctrl+Shift+D", "", 40 },
        { "studio.keys.testing", UMI_STUDIO_ACTIVITY_TEST,
          "Ctrl+Alt+T", "", 50 },
        { "studio.keys.designer", UMI_STUDIO_ACTIVITY_DESIGNER,
          "Ctrl+Alt+D", "studio.designer.available", 60 },
        { "studio.keys.applications", UMI_STUDIO_ACTIVITY_APPLICATIONS,
          "Ctrl+Alt+A", "studio.applications.available", 70 },
        { "studio.keys.framework", UMI_STUDIO_ACTIVITY_FRAMEWORK,
          "Ctrl+Alt+F", "", 80 },
        { "studio.keys.ai", UMI_STUDIO_ACTIVITY_AI,
          "Ctrl+Alt+I", "studio.ai.available", 90 }
    };
    size_t index;
    UmiStatus status;

    for (index = 0U;
         index < sizeof(ACTIVATION_KEYS) / sizeof(ACTIVATION_KEYS[0]);
         ++index) {
        status = register_keybinding(
            workbench,
            ACTIVATION_KEYS[index].id,
            UMI_STUDIO_COMMAND_ACTIVITY_ACTIVATE,
            ACTIVATION_KEYS[index].argument,
            ACTIVATION_KEYS[index].chord,
            ACTIVATION_KEYS[index].when_expression,
            ACTIVATION_KEYS[index].order);
        if (status != UMI_STATUS_OK) return status;
    }

    status = register_keybinding(workbench,
                                 "studio.keys.sidebar-toggle",
                                 UMI_STUDIO_COMMAND_SIDEBAR_TOGGLE,
                                 "",
                                 "Ctrl+B",
                                 "",
                                 100);
    if (status != UMI_STATUS_OK) return status;

    status = register_keybinding(workbench,
                                 "studio.keys.bottom-toggle",
                                 UMI_STUDIO_COMMAND_BOTTOM_PANEL_TOGGLE,
                                 "",
                                 "Ctrl+J",
                                 "",
                                 110);
    if (status != UMI_STATUS_OK) return status;

    status = register_keybinding(workbench,
                                 "studio.keys.auxiliary-toggle",
                                 UMI_STUDIO_COMMAND_AUXILIARY_TOGGLE,
                                 "",
                                 "Ctrl+Alt+B",
                                 "",
                                 120);
    if (status != UMI_STATUS_OK) return status;

    status = register_keybinding(workbench,
                                 "studio.keys.command-palette",
                                 UMI_STUDIO_COMMAND_QUICK_ACCESS_SHOW,
                                 "",
                                 "Ctrl+Shift+P",
                                 "",
                                 130);
    if (status != UMI_STATUS_OK) return status;

    status = register_keybinding(workbench,
                                 "studio.keys.editor-next",
                                 UMI_STUDIO_COMMAND_EDITOR_NEXT,
                                 "",
                                 "Ctrl+Tab",
                                 "",
                                 140);
    if (status != UMI_STATUS_OK) return status;

    status = register_keybinding(workbench,
                                 "studio.keys.editor-previous",
                                 UMI_STUDIO_COMMAND_EDITOR_PREVIOUS,
                                 "",
                                 "Ctrl+Shift+Tab",
                                 "",
                                 150);
    if (status != UMI_STATUS_OK) return status;

    status = register_keybinding(workbench,
                                 "studio.keys.editor-split-right",
                                 UMI_STUDIO_COMMAND_EDITOR_SPLIT_RIGHT,
                                 "",
                                 "Ctrl+\\",
                                 "",
                                 160);
    if (status != UMI_STATUS_OK) return status;

    status = register_keybinding(workbench,
                                 "studio.keys.editor-split-down",
                                 UMI_STUDIO_COMMAND_EDITOR_SPLIT_DOWN,
                                 "",
                                 "Ctrl+Shift+\\",
                                 "",
                                 170);
    if (status != UMI_STATUS_OK) return status;

    status = register_keybinding(workbench,
                                 "studio.keys.editor-move-next-group",
                                 UMI_STUDIO_COMMAND_EDITOR_MOVE_NEXT_GROUP,
                                 "",
                                 "Ctrl+Alt+Right",
                                 "",
                                 180);
    if (status != UMI_STATUS_OK) return status;

    status = register_keybinding(workbench,
                                 "studio.keys.editor-move-previous-group",
                                 UMI_STUDIO_COMMAND_EDITOR_MOVE_PREVIOUS_GROUP,
                                 "",
                                 "Ctrl+Alt+Left",
                                 "",
                                 190);
    if (status != UMI_STATUS_OK) return status;

    status = register_keybinding(workbench,
                                 "studio.keys.editor-focus-next-group",
                                 UMI_STUDIO_COMMAND_EDITOR_FOCUS_NEXT_GROUP,
                                 "",
                                 "Ctrl+Alt+G",
                                 "",
                                 200);
    if (status != UMI_STATUS_OK) return status;

    return register_keybinding(workbench,
                               "studio.keys.editor-word-wrap",
                               UMI_STUDIO_COMMAND_EDITOR_WORD_WRAP_TOGGLE,
                               "",
                               "Alt+Z",
                               "",
                               210);
}

static UmiStatus seed_explorer(UmiUiWorkbench *workbench)
{
    static const struct {
        const char *id;
        const char *parent;
        const char *name;
        const char *path;
        UmiUiExplorerNodeKind kind;
        uint16_t depth;
    } NODES[] = {
        { "studio.workspace.root", "", "Umicom Studio IDE", ".",
          UMI_UI_EXPLORER_WORKSPACE, 0U },
        { "studio.workspace.applications", "studio.workspace.root",
          "applications", "applications", UMI_UI_EXPLORER_FOLDER, 1U },
        { "studio.workspace.studio", "studio.workspace.applications",
          "studio", "applications/studio", UMI_UI_EXPLORER_FOLDER, 2U },
        { "studio.workspace.framework", "studio.workspace.root",
          "framework", "framework", UMI_UI_EXPLORER_FOLDER, 1U },
        { "studio.workspace.docs", "studio.workspace.root",
          "docs", "docs", UMI_UI_EXPLORER_FOLDER, 1U }
    };
    size_t index;
    UmiStatus status;

    for (index = 0U; index < sizeof(NODES) / sizeof(NODES[0]); ++index) {
        UmiUiExplorerNode node = {0};
        (void)snprintf(node.node_id, sizeof(node.node_id), "%s", NODES[index].id);
        (void)snprintf(node.parent_id, sizeof(node.parent_id), "%s", NODES[index].parent);
        (void)snprintf(node.name, sizeof(node.name), "%s", NODES[index].name);
        (void)snprintf(node.path, sizeof(node.path), "%s", NODES[index].path);
        node.kind = NODES[index].kind;
        node.depth = NODES[index].depth;
        node.expanded = node.kind != UMI_UI_EXPLORER_FILE;
        status = umi_ui_explorer_model_upsert(
            umi_ui_workbench_explorer(workbench), &node);
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_ui_explorer_model_select(umi_ui_workbench_explorer(workbench),
                                        "studio.workspace.studio");
}

UmiStatus umi_studio_workbench_shell_catalogue_register(
    UmiUiWorkbench *workbench,
    UmiStudioServices *services)
{
    UmiStatus status;
    (void)services;
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = register_view_containers(workbench);
    if (status == UMI_STATUS_OK) status = register_activities(workbench);
    if (status == UMI_STATUS_OK) status = register_context(workbench);
    if (status == UMI_STATUS_OK) status = register_keybindings(workbench);
    if (status == UMI_STATUS_OK) status = seed_explorer(workbench);
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workbench_set_breadcrumb_path(
            workbench, "Umicom Studio IDE/applications/studio");
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workbench_activate_activity(workbench,
                                                    UMI_STUDIO_ACTIVITY_EXPLORER);
    }
    return status;
}
