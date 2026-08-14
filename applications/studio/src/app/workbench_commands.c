/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/workbench_commands.c
 *
 * PURPOSE:
 *   Implement and register Studio workbench commands against the authoritative
 *   Framework command registry and unified workbench services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Activity buttons, menus, keyboard shortcuts and the command palette all end
 * here. The GTK4 frontend never owns a second implementation of the command.
 */

#include "umicom/studio/workbench_commands.h"

#include <stdio.h>
#include <string.h>

#include "umicom/studio/ui.h"
#include "umicom/studio/workbench.h"

static UmiStatus perspective_activate(void *user_data,
                                      const char *argument,
                                      char *out_message,
                                      size_t capacity)
{
    UmiStudioUi *ui = (UmiStudioUi *)user_data;
    UmiStatus status;
    if (argument == NULL || argument[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_workbench_activate_perspective(
        umi_studio_ui_workbench(ui), argument);
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s",
                       status == UMI_STATUS_OK ? argument : umi_status_text(status));
    }
    return status;
}

static UmiStatus activity_activate(void *user_data,
                                   const char *argument,
                                   char *out_message,
                                   size_t capacity)
{
    UmiStudioUi *ui = (UmiStudioUi *)user_data;
    UmiUiWorkbench *workbench;
    UmiStatus status;
    if (ui == NULL || argument == NULL || argument[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    workbench = umi_studio_ui_workbench(ui);
    status = umi_ui_workbench_activate_activity(workbench, argument);
    if (status == UMI_STATUS_OK) {
        UmiUiWorkbenchSnapshot snapshot;
        if (umi_ui_workbench_snapshot(workbench, &snapshot) == UMI_STATUS_OK) {
            (void)umi_ui_context_set_string(
                umi_ui_workbench_context(workbench),
                "studio.ui.active-activity",
                snapshot.active_activity);
        }
    }
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s",
                       status == UMI_STATUS_OK ? argument : umi_status_text(status));
    }
    return status;
}

static UmiStatus pane_toggle(void *user_data,
                             const char *argument,
                             char *out_message,
                             size_t capacity)
{
    UmiStudioUi *ui = (UmiStudioUi *)user_data;
    UmiUiPaneSnapshot pane;
    UmiStatus status;
    if (argument == NULL || argument[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_pane_model_find(
        umi_ui_workbench_panes(umi_studio_ui_workbench(ui)), argument, &pane);
    if (status != UMI_STATUS_OK) return status;
    pane.visible = !pane.visible;
    status = umi_ui_pane_model_upsert(
        umi_ui_workbench_panes(umi_studio_ui_workbench(ui)), &pane);
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s %s", pane.title,
                       pane.visible ? "visible" : "hidden");
    }
    return status;
}

static UmiStatus chrome_toggle(UmiStudioUi *ui,
                               int toggle_sidebar,
                               int toggle_bottom,
                               int toggle_auxiliary,
                               char *out_message,
                               size_t capacity)
{
    UmiUiWorkbench *workbench;
    UmiUiWorkbenchState state;
    UmiStatus status;

    if (ui == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    workbench = umi_studio_ui_workbench(ui);
    status = umi_ui_workbench_state_snapshot(workbench, &state);
    if (status != UMI_STATUS_OK) return status;

    if (toggle_sidebar) state.sidebar_visible = !state.sidebar_visible;
    if (toggle_bottom) state.bottom_panel_visible = !state.bottom_panel_visible;
    if (toggle_auxiliary) {
        state.auxiliary_sidebar_visible = !state.auxiliary_sidebar_visible;
    }

    status = umi_ui_workbench_state_apply(workbench, &state);
    if (status != UMI_STATUS_OK) return status;

    (void)umi_ui_context_set_boolean(umi_ui_workbench_context(workbench),
                                     "studio.ui.sidebar.visible",
                                     state.sidebar_visible);
    (void)umi_ui_context_set_boolean(umi_ui_workbench_context(workbench),
                                     "studio.ui.bottom.visible",
                                     state.bottom_panel_visible);
    (void)umi_ui_context_set_boolean(umi_ui_workbench_context(workbench),
                                     "studio.ui.auxiliary.visible",
                                     state.auxiliary_sidebar_visible);

    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity,
                       "Sidebar=%s Bottom=%s Auxiliary=%s",
                       state.sidebar_visible ? "visible" : "hidden",
                       state.bottom_panel_visible ? "visible" : "hidden",
                       state.auxiliary_sidebar_visible ? "visible" : "hidden");
    }
    return UMI_STATUS_OK;
}

static UmiStatus sidebar_toggle(void *user_data,
                                const char *argument,
                                char *out_message,
                                size_t capacity)
{
    (void)argument;
    return chrome_toggle((UmiStudioUi *)user_data, 1, 0, 0,
                         out_message, capacity);
}

static UmiStatus bottom_panel_toggle(void *user_data,
                                     const char *argument,
                                     char *out_message,
                                     size_t capacity)
{
    (void)argument;
    return chrome_toggle((UmiStudioUi *)user_data, 0, 1, 0,
                         out_message, capacity);
}

static UmiStatus auxiliary_toggle(void *user_data,
                                  const char *argument,
                                  char *out_message,
                                  size_t capacity)
{
    (void)argument;
    return chrome_toggle((UmiStudioUi *)user_data, 0, 0, 1,
                         out_message, capacity);
}

static UmiStatus status_set(void *user_data,
                            const char *argument,
                            char *out_message,
                            size_t capacity)
{
    UmiStudioUi *ui = (UmiStudioUi *)user_data;
    UmiUiStatusSnapshot item = {0};
    if (argument == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)snprintf(item.item_id, sizeof(item.item_id), "%s", "studio.status.ready");
    (void)snprintf(item.text, sizeof(item.text), "%s", argument);
    (void)snprintf(item.tooltip, sizeof(item.tooltip), "%s", "Studio runtime status");
    item.priority = 100;
    item.visible = 1;
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s", argument);
    }
    return umi_ui_status_model_upsert(
        umi_ui_workbench_status(umi_studio_ui_workbench(ui)), &item);
}

static UmiStatus layout_reset(void *user_data,
                              const char *argument,
                              char *out_message,
                              size_t capacity)
{
    UmiStatus status;
    (void)argument;
    status = umi_studio_workbench_reset_layout(
        umi_studio_ui_workbench((UmiStudioUi *)user_data));
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s",
                       status == UMI_STATUS_OK
                           ? "Studio layout reset"
                           : umi_status_text(status));
    }
    return status;
}

static UmiStatus workspace_profile_activate(void *user_data,
                                            const char *argument,
                                            char *out_message,
                                            size_t capacity)
{
    UmiStudioUi *ui = (UmiStudioUi *)user_data;
    UmiUiWorkbench *workbench;
    UmiStatus status;

    if (ui == NULL || argument == NULL || argument[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    workbench = umi_studio_ui_workbench(ui);
    status = umi_ui_workbench_activate_workspace_profile(workbench, argument);
    if (status == UMI_STATUS_OK) {
        (void)umi_ui_context_set_string(
            umi_ui_workbench_context(workbench),
            "studio.ui.workspace-profile",
            argument);
    }
    if (out_message != NULL && capacity > 0U) {
        if (status == UMI_STATUS_OK) {
            (void)snprintf(out_message, capacity,
                           "Workspace profile: %s", argument);
        } else {
            (void)snprintf(out_message, capacity, "%s",
                           umi_status_text(status));
        }
    }
    return status;
}

static UmiStatus notification_info(void *user_data,
                                   const char *argument,
                                   char *out_message,
                                   size_t capacity)
{
    UmiStudioUi *ui = (UmiStudioUi *)user_data;
    UmiUiNotification notification = {0};
    UmiStatus status;
    notification.severity = UMI_UI_SEVERITY_INFORMATION;
    (void)snprintf(notification.title, sizeof(notification.title), "%s",
                   "Umicom Studio IDE");
    (void)snprintf(notification.message, sizeof(notification.message), "%s",
                   argument != NULL && argument[0] != '\0'
                       ? argument : "Studio notification");
    status = umi_ui_notification_publish(
        umi_ui_workbench_notifications(umi_studio_ui_workbench(ui)),
        &notification, NULL);
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s",
                       status == UMI_STATUS_OK
                           ? "Notification published"
                           : umi_status_text(status));
    }
    return status;
}

UmiStatus umi_studio_workbench_commands_register(UmiCommandRegistry *registry,
                                                  UmiStudioUi *ui)
{
    static const struct {
        const char *id;
        const char *title;
        const char *description;
        UmiRegisteredCommandHandler handler;
        uint32_t flags;
    } DEFINITIONS[] = {
        { UMI_STUDIO_COMMAND_PERSPECTIVE_ACTIVATE,
          "Activate Perspective", "Activate a Studio workbench perspective",
          perspective_activate, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_ACTIVITY_ACTIVATE,
          "Activate Activity", "Activate a Studio Activity Bar destination",
          activity_activate, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_PANE_TOGGLE,
          "Toggle Pane", "Show or hide a Studio pane",
          pane_toggle, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_SIDEBAR_TOGGLE,
          "Toggle Primary Sidebar", "Show or hide the primary workbench sidebar",
          sidebar_toggle, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_BOTTOM_PANEL_TOGGLE,
          "Toggle Bottom Panel", "Show or hide Output, Problems and Terminal",
          bottom_panel_toggle, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_AUXILIARY_TOGGLE,
          "Toggle Auxiliary Sidebar", "Show or hide the right-hand utility sidebar",
          auxiliary_toggle, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_STATUS_SET,
          "Set Status", "Set the primary Studio status text",
          status_set, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_LAYOUT_RESET,
          "Reset Layout", "Restore the default Studio workbench layout",
          layout_reset, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_WORKSPACE_PROFILE_ACTIVATE,
          "Activate Workspace Profile",
          "Switch the visible Studio tool regions to a named workspace profile",
          workspace_profile_activate, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_NOTIFICATION_INFO,
          "Show Notification", "Publish an information notification",
          notification_info, UMI_COMMAND_MUTATES_STATE }
    };
    size_t index;
    UmiStatus status;
    UmiCommandDescriptor descriptor;

    if (registry == NULL || ui == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < sizeof(DEFINITIONS) / sizeof(DEFINITIONS[0]); ++index) {
        (void)memset(&descriptor, 0, sizeof(descriptor));
        descriptor.structure_size = (uint32_t)sizeof(descriptor);
        descriptor.command_id = DEFINITIONS[index].id;
        descriptor.title = DEFINITIONS[index].title;
        descriptor.category = "Studio UI";
        descriptor.description = DEFINITIONS[index].description;
        descriptor.required_permission = "studio.ui.use";
        descriptor.flags = DEFINITIONS[index].flags;
        descriptor.handler = DEFINITIONS[index].handler;
        descriptor.user_data = ui;
        status = umi_command_registry_register(registry, &descriptor);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
