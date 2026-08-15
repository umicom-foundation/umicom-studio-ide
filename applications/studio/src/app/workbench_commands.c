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

#include <stdint.h>
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

static UmiStatus quick_access_show(void *user_data,
                                   const char *argument,
                                   char *out_message,
                                   size_t capacity)
{
    UmiStudioUi *ui = (UmiStudioUi *)user_data;
    UmiUiContextStore *context;
    UmiUiContextSnapshot current;
    int64_t request = 1;
    UmiStatus status;
    (void)argument;
    if (ui == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context = umi_ui_workbench_context(umi_studio_ui_workbench(ui));
    if (umi_ui_context_get(context,
                           UMI_UI_QUICK_ACCESS_REQUEST_CONTEXT_KEY,
                           &current) == UMI_STATUS_OK &&
        current.kind == UMI_UI_CONTEXT_INTEGER &&
        current.integer_value < INT64_MAX) {
        request = current.integer_value + 1;
    }
    status = umi_ui_context_set_integer(
        context, UMI_UI_QUICK_ACCESS_REQUEST_CONTEXT_KEY, request);
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s",
                       status == UMI_STATUS_OK
                           ? "Command palette ready"
                           : umi_status_text(status));
    }
    return status;
}

static UmiStatus active_document(UmiStudioUi *ui,
                                 UmiUiWorkbench **out_workbench,
                                 UmiUiDocumentViewSnapshot *out_document)
{
    UmiUiWorkbenchSnapshot snapshot;
    UmiUiWorkbench *workbench;
    UmiStatus status;
    if (ui == NULL || out_workbench == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    workbench = umi_studio_ui_workbench(ui);
    status = umi_ui_workbench_snapshot(workbench, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (snapshot.active_document_view[0] == '\0') return UMI_STATUS_NOT_FOUND;
    status = umi_ui_document_view_model_find(
        umi_ui_workbench_documents(workbench),
        snapshot.active_document_view,
        out_document);
    if (status == UMI_STATUS_OK) *out_workbench = workbench;
    return status;
}

static UmiStatus editor_close(void *user_data,
                              int close_all,
                              char *out_message,
                              size_t capacity)
{
    UmiStudioUi *ui = (UmiStudioUi *)user_data;
    UmiUiWorkbench *workbench = NULL;
    UmiUiDocumentViewSnapshot document;
    UmiUiDocumentCloseResult result = {0};
    UmiStatus status;
    status = active_document(ui, &workbench, &document);
    if (status != UMI_STATUS_OK) return status;
    status = close_all
        ? umi_ui_document_view_model_close_all(
              umi_ui_workbench_documents(workbench), &result)
        : umi_ui_document_view_model_close_others(
              umi_ui_workbench_documents(workbench),
              document.view_id,
              &result);
    if (status == UMI_STATUS_OK && close_all &&
        umi_ui_document_view_model_find(
            umi_ui_workbench_documents(workbench),
            document.view_id,
            &document) == UMI_STATUS_NOT_FOUND &&
        umi_ui_document_view_model_count(
            umi_ui_workbench_documents(workbench)) > 0U &&
        umi_ui_document_view_model_at(
            umi_ui_workbench_documents(workbench),
            0U,
            &document) == UMI_STATUS_OK) {
        status = umi_ui_workbench_activate_document(workbench,
                                                    document.view_id);
    }
    if (status == UMI_STATUS_OK && out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity,
                       "Closed %zu editor(s); preserved %zu dirty, %zu pinned and %zu protected",
                       result.closed_count,
                       result.dirty_count,
                       result.pinned_count,
                       result.non_closable_count);
    }
    return status;
}

static UmiStatus editor_close_others(void *user_data,
                                     const char *argument,
                                     char *out_message,
                                     size_t capacity)
{
    (void)argument;
    return editor_close(user_data, 0, out_message, capacity);
}

static UmiStatus editor_close_all(void *user_data,
                                  const char *argument,
                                  char *out_message,
                                  size_t capacity)
{
    (void)argument;
    return editor_close(user_data, 1, out_message, capacity);
}

static UmiStatus editor_pin_toggle(void *user_data,
                                   const char *argument,
                                   char *out_message,
                                   size_t capacity)
{
    UmiUiWorkbench *workbench = NULL;
    UmiUiDocumentViewSnapshot document;
    UmiStatus status;
    (void)argument;
    status = active_document((UmiStudioUi *)user_data,
                             &workbench,
                             &document);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_document_view_model_set_pinned(
        umi_ui_workbench_documents(workbench),
        document.view_id,
        !document.pinned);
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s: %s",
                       document.title,
                       document.pinned ? "unpinned" : "pinned");
    }
    return status;
}

static UmiStatus editor_preview_promote(void *user_data,
                                        const char *argument,
                                        char *out_message,
                                        size_t capacity)
{
    UmiUiWorkbench *workbench = NULL;
    UmiUiDocumentViewSnapshot document;
    UmiStatus status;
    (void)argument;
    status = active_document((UmiStudioUi *)user_data,
                             &workbench,
                             &document);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_document_view_model_promote_preview(
        umi_ui_workbench_documents(workbench), document.view_id);
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity,
                       "%s is now a permanent editor", document.title);
    }
    return status;
}

static UmiStatus editor_word_wrap_toggle(void *user_data,
                                         const char *argument,
                                         char *out_message,
                                         size_t capacity)
{
    UmiUiWorkbench *workbench = NULL;
    UmiUiDocumentViewSnapshot document;
    UmiStatus status;
    (void)argument;
    status = active_document((UmiStudioUi *)user_data,
                             &workbench,
                             &document);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_document_view_model_set_word_wrap(
        umi_ui_workbench_documents(workbench),
        document.view_id,
        !document.word_wrap);
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "Word wrap %s",
                       document.word_wrap ? "disabled" : "enabled");
    }
    return status;
}

static UmiStatus editor_navigate(void *user_data,
                                 int direction,
                                 char *out_message,
                                 size_t capacity)
{
    UmiUiWorkbench *workbench = NULL;
    UmiUiDocumentViewSnapshot document;
    char target[UMI_UI_ID_CAPACITY];
    UmiStatus status;
    status = active_document((UmiStudioUi *)user_data,
                             &workbench,
                             &document);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_document_view_model_activate_relative(
        umi_ui_workbench_documents(workbench),
        document.view_id,
        direction,
        target,
        sizeof(target));
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workbench_activate_document(workbench, target);
    }
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s",
                       status == UMI_STATUS_OK ? target : umi_status_text(status));
    }
    return status;
}

static UmiStatus editor_next(void *user_data,
                             const char *argument,
                             char *out_message,
                             size_t capacity)
{
    (void)argument;
    return editor_navigate(user_data, 1, out_message, capacity);
}

static UmiStatus editor_previous(void *user_data,
                                 const char *argument,
                                 char *out_message,
                                 size_t capacity)
{
    (void)argument;
    return editor_navigate(user_data, -1, out_message, capacity);
}

static const char *other_editor_group(const char *group_id)
{
    return strcmp(group_id, UMI_UI_SECONDARY_EDITOR_GROUP_ID) == 0
        ? UMI_UI_PRIMARY_EDITOR_GROUP_ID
        : UMI_UI_SECONDARY_EDITOR_GROUP_ID;
}

static UmiStatus editor_split(void *user_data,
                              UmiUiEditorSplitMode mode,
                              char *out_message,
                              size_t capacity)
{
    UmiUiWorkbench *workbench = NULL;
    UmiUiDocumentViewSnapshot document;
    UmiUiWorkbenchState state;
    UmiStatus status = active_document((UmiStudioUi *)user_data,
                                       &workbench, &document);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workbench_state_snapshot(workbench, &state);
    if (status != UMI_STATUS_OK) return status;
    state.editor_split_mode = mode;
    state.editor_split_ratio = UMI_UI_EDITOR_SPLIT_RATIO_DEFAULT;
    if (strcmp(document.group_id,
               UMI_UI_SECONDARY_EDITOR_GROUP_ID) != 0) {
        status = umi_ui_document_view_model_move_to_group(
            umi_ui_workbench_documents(workbench), document.view_id,
            UMI_UI_SECONDARY_EDITOR_GROUP_ID);
        if (status != UMI_STATUS_OK) return status;
    }
    (void)snprintf(state.active_editor_group,
                   sizeof(state.active_editor_group), "%s",
                   UMI_UI_SECONDARY_EDITOR_GROUP_ID);
    status = umi_ui_workbench_state_apply(workbench, &state);
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workbench_activate_document(workbench,
                                                    document.view_id);
    }
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s",
                       status == UMI_STATUS_OK
                           ? (mode == UMI_UI_EDITOR_SPLIT_COLUMNS
                                  ? "Editor split into columns"
                                  : "Editor split into rows")
                           : umi_status_text(status));
    }
    return status;
}

static UmiStatus editor_split_right(void *user_data,
                                    const char *argument,
                                    char *out_message,
                                    size_t capacity)
{
    (void)argument;
    return editor_split(user_data, UMI_UI_EDITOR_SPLIT_COLUMNS,
                        out_message, capacity);
}

static UmiStatus editor_split_down(void *user_data,
                                   const char *argument,
                                   char *out_message,
                                   size_t capacity)
{
    (void)argument;
    return editor_split(user_data, UMI_UI_EDITOR_SPLIT_ROWS,
                        out_message, capacity);
}

static UmiStatus editor_move_group(void *user_data,
                                   char *out_message,
                                   size_t capacity)
{
    UmiUiWorkbench *workbench = NULL;
    UmiUiDocumentViewSnapshot document;
    UmiUiWorkbenchState state;
    const char *target_group;
    UmiStatus status = active_document((UmiStudioUi *)user_data,
                                       &workbench, &document);
    if (status != UMI_STATUS_OK) return status;
    target_group = other_editor_group(document.group_id);
    status = umi_ui_document_view_model_move_to_group(
        umi_ui_workbench_documents(workbench), document.view_id,
        target_group);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workbench_state_snapshot(workbench, &state);
    if (status != UMI_STATUS_OK) return status;
    if (state.editor_split_mode == UMI_UI_EDITOR_SPLIT_SINGLE) {
        state.editor_split_mode = UMI_UI_EDITOR_SPLIT_COLUMNS;
    }
    (void)snprintf(state.active_editor_group,
                   sizeof(state.active_editor_group), "%s", target_group);
    status = umi_ui_workbench_state_apply(workbench, &state);
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workbench_activate_document(workbench,
                                                    document.view_id);
    }
    if (out_message != NULL && capacity > 0U) {
        if (status == UMI_STATUS_OK) {
            (void)snprintf(out_message, capacity, "Moved %s to %s",
                           document.title, target_group);
        } else {
            (void)snprintf(out_message, capacity, "%s",
                           umi_status_text(status));
        }
    }
    return status;
}

static UmiStatus editor_move_next_group(void *user_data,
                                        const char *argument,
                                        char *out_message,
                                        size_t capacity)
{
    (void)argument;
    return editor_move_group(user_data, out_message, capacity);
}

static UmiStatus editor_move_previous_group(void *user_data,
                                            const char *argument,
                                            char *out_message,
                                            size_t capacity)
{
    (void)argument;
    return editor_move_group(user_data, out_message, capacity);
}

static UmiStatus editor_focus_next_group(void *user_data,
                                         const char *argument,
                                         char *out_message,
                                         size_t capacity)
{
    UmiStudioUi *ui = (UmiStudioUi *)user_data;
    UmiUiWorkbench *workbench;
    UmiUiWorkbenchState state;
    const char *target_group;
    char target_view[UMI_UI_ID_CAPACITY];
    UmiStatus status;
    (void)argument;
    if (ui == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    workbench = umi_studio_ui_workbench(ui);
    status = umi_ui_workbench_state_snapshot(workbench, &state);
    if (status != UMI_STATUS_OK) return status;
    target_group = other_editor_group(state.active_editor_group);
    status = umi_ui_document_view_model_activate_group(
        umi_ui_workbench_documents(workbench), target_group,
        target_view, sizeof(target_view));
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workbench_activate_document(workbench, target_view);
    }
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s",
                       status == UMI_STATUS_OK
                           ? target_view
                           : umi_status_text(status));
    }
    return status;
}

static UmiStatus editor_balance_groups(void *user_data,
                                       const char *argument,
                                       char *out_message,
                                       size_t capacity)
{
    UmiStudioUi *ui = (UmiStudioUi *)user_data;
    UmiUiWorkbench *workbench;
    UmiUiWorkbenchState state;
    UmiStatus status;
    (void)argument;
    if (ui == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    workbench = umi_studio_ui_workbench(ui);
    status = umi_ui_workbench_state_snapshot(workbench, &state);
    if (status != UMI_STATUS_OK) return status;
    state.editor_split_ratio = UMI_UI_EDITOR_SPLIT_RATIO_DEFAULT;
    status = umi_ui_workbench_state_apply(workbench, &state);
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s",
                       status == UMI_STATUS_OK
                           ? "Editor groups balanced"
                           : umi_status_text(status));
    }
    return status;
}

static UmiStatus editor_reset_groups(void *user_data,
                                     const char *argument,
                                     char *out_message,
                                     size_t capacity)
{
    UmiStudioUi *ui = (UmiStudioUi *)user_data;
    UmiUiWorkbench *workbench;
    UmiUiWorkbenchState state;
    char target_view[UMI_UI_ID_CAPACITY];
    UmiStatus status;
    (void)argument;
    if (ui == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    workbench = umi_studio_ui_workbench(ui);
    status = umi_ui_document_view_model_merge_group(
        umi_ui_workbench_documents(workbench),
        UMI_UI_SECONDARY_EDITOR_GROUP_ID,
        UMI_UI_PRIMARY_EDITOR_GROUP_ID);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workbench_state_snapshot(workbench, &state);
    if (status != UMI_STATUS_OK) return status;
    state.editor_split_mode = UMI_UI_EDITOR_SPLIT_SINGLE;
    state.editor_split_ratio = UMI_UI_EDITOR_SPLIT_RATIO_DEFAULT;
    (void)snprintf(state.active_editor_group,
                   sizeof(state.active_editor_group), "%s",
                   UMI_UI_PRIMARY_EDITOR_GROUP_ID);
    status = umi_ui_workbench_state_apply(workbench, &state);
    if (status == UMI_STATUS_OK &&
        umi_ui_document_view_model_activate_group(
            umi_ui_workbench_documents(workbench),
            UMI_UI_PRIMARY_EDITOR_GROUP_ID,
            target_view, sizeof(target_view)) == UMI_STATUS_OK) {
        status = umi_ui_workbench_activate_document(workbench, target_view);
    }
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s",
                       status == UMI_STATUS_OK
                           ? "Editor groups reset"
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
          notification_info, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_QUICK_ACCESS_SHOW,
          "Show Command Palette",
          "Focus the global action-aware command palette",
          quick_access_show, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_EDITOR_CLOSE_OTHERS,
          "Close Other Editors",
          "Close clean, unpinned editors in the active editor group",
          editor_close_others, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_EDITOR_CLOSE_ALL,
          "Close All Editors",
          "Close clean, unpinned editors while preserving unsaved work",
          editor_close_all, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_EDITOR_PIN_TOGGLE,
          "Pin or Unpin Editor",
          "Toggle persistence of the active editor tab",
          editor_pin_toggle, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_EDITOR_PREVIEW_PROMOTE,
          "Keep Preview Editor",
          "Promote the active preview into a permanent editor tab",
          editor_preview_promote, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_EDITOR_WORD_WRAP_TOGGLE,
          "Toggle Word Wrap",
          "Toggle viewport word wrapping in the active editor",
          editor_word_wrap_toggle, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_EDITOR_NEXT,
          "Next Editor",
          "Activate the next editor in the current group",
          editor_next, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_EDITOR_PREVIOUS,
          "Previous Editor",
          "Activate the previous editor in the current group",
          editor_previous, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_EDITOR_SPLIT_RIGHT,
          "Split Editor Right", "Open the active editor in a right-hand group",
          editor_split_right, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_EDITOR_SPLIT_DOWN,
          "Split Editor Down", "Open the active editor in a lower group",
          editor_split_down, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_EDITOR_MOVE_NEXT_GROUP,
          "Move Editor to Next Group", "Move the active tab to the other group",
          editor_move_next_group, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_EDITOR_MOVE_PREVIOUS_GROUP,
          "Move Editor to Previous Group", "Move the active tab to the other group",
          editor_move_previous_group, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_EDITOR_FOCUS_NEXT_GROUP,
          "Focus Next Editor Group", "Activate the other editor group",
          editor_focus_next_group, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_EDITOR_BALANCE_GROUPS,
          "Balance Editor Groups", "Give both editor groups equal space",
          editor_balance_groups, UMI_COMMAND_MUTATES_STATE },
        { UMI_STUDIO_COMMAND_EDITOR_RESET_GROUPS,
          "Reset Editor Groups", "Merge all editors into one primary group",
          editor_reset_groups, UMI_COMMAND_MUTATES_STATE }
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
