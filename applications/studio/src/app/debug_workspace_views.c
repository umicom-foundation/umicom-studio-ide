/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/debug_workspace_views.c
 *
 * PURPOSE:
 *   Present Framework debugger records as dedicated Variables, Watch, Call
 *   Stack, Breakpoints and Debug Console view models for every frontend.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/debug_workspace_views.h"

#include <stdio.h>
#include <string.h>

#define DEBUG_VIEW_ROW_LIMIT 8U

static UmiStatus set_string(UmiUiViewModel *view, const char *key,
                            const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(&value,
                                               text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_integer(UmiUiViewModel *view, const char *key,
                             int64_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus create_view(const char *view_id, const char *view_type,
                             const char *title, const char *summary,
                             UmiUiViewModel **out_view)
{
    UmiStatus status;

    if (view_id == NULL || view_type == NULL || out_view == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ui_view_model_create(view_id, view_type, UMI_UI_ROLE_PANE,
                                      out_view);
    if (status == UMI_STATUS_OK) {
        status = set_string(*out_view, "title", title);
    }
    if (status == UMI_STATUS_OK) {
        status = set_string(*out_view, "summary", summary);
    }
    if (status != UMI_STATUS_OK && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

static UmiStatus add_action(UmiUiViewModel *view, size_t index,
                            const char *action_id, const char *label,
                            const char *tooltip)
{
    UmiUiCommandViewAction action = {0};

    (void)snprintf(action.action_id, sizeof(action.action_id), "%s",
                   action_id);
    (void)snprintf(action.label, sizeof(action.label), "%s", label);
    (void)snprintf(action.tooltip, sizeof(action.tooltip), "%s", tooltip);
    action.enabled = 1;
    return umi_ui_command_view_set_action(view, index, &action);
}

static UmiStatus workspace_snapshot(UmiStudioDebuggerService *debugger,
                                    UmiDebugWorkspace **out_workspace,
                                    UmiDebugWorkspaceSnapshot *out_snapshot)
{
    if (debugger == NULL || out_workspace == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_workspace = umi_studio_debugger_service_workspace(debugger);
    return *out_workspace != NULL
        ? umi_debug_workspace_snapshot(*out_workspace, out_snapshot)
        : UMI_STATUS_UNAVAILABLE;
}

UmiStatus umi_studio_debug_call_stack_view_create(
    const char *view_id, UmiStudioDebuggerService *debugger,
    UmiUiViewModel **out_view)
{
    UmiDebugWorkspace *workspace;
    UmiDebugWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "studio.debug-call-stack", "Call Stack",
        "Threads and stack frames for the selected debug session.", out_view);
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    status = workspace_snapshot(debugger, &workspace, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    status = set_string(*out_view, "state", snapshot.controller_state_label);
    if (status == UMI_STATUS_OK) {
        status = set_string(*out_view, "selected-thread",
                            snapshot.selected_thread_id);
    }
    if (status == UMI_STATUS_OK) {
        status = set_string(*out_view, "selected-frame",
                            snapshot.selected_frame_id);
    }
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view, "threads",
                             (int64_t)snapshot.thread_count);
    }
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view, "frames",
                             (int64_t)snapshot.visible_frame_count);
    }
    for (index = 0U; status == UMI_STATUS_OK &&
         index < snapshot.thread_count && index < DEBUG_VIEW_ROW_LIMIT;
         ++index) {
        UmiDebugThreadSnapshot thread;
        char key[32];
        char text[768];
        status = umi_debug_workspace_thread_at(workspace, index, &thread);
        if (status == UMI_STATUS_OK) {
            (void)snprintf(key, sizeof(key), "thread-%02zu", index + 1U);
            (void)snprintf(text, sizeof(text), "%s%s — %s", thread.current
                           ? "● " : "", thread.name, thread.stopped
                           ? "paused" : "running");
            status = set_string(*out_view, key, text);
        }
    }
    for (index = 0U; status == UMI_STATUS_OK &&
         index < snapshot.visible_frame_count && index < DEBUG_VIEW_ROW_LIMIT;
         ++index) {
        UmiDebugStackFrameSnapshot frame;
        char key[32];
        char text[1536];
        status = umi_debug_workspace_frame_at(workspace, index, &frame);
        if (status == UMI_STATUS_OK) {
            (void)snprintf(key, sizeof(key), "frame-%02zu", index + 1U);
            (void)snprintf(text, sizeof(text), "#%d %s — %s:%u",
                           frame.order, frame.name, frame.source_uri,
                           frame.line);
            status = set_string(*out_view, key, text);
        }
    }
    return status;
}

UmiStatus umi_studio_debug_variables_view_create(
    const char *view_id, UmiStudioDebuggerService *debugger,
    UmiUiViewModel **out_view)
{
    UmiDebugWorkspace *workspace;
    UmiDebugWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "studio.debug-variables", "Variables",
        "Locals and scoped values for the selected stack frame.", out_view);
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    status = workspace_snapshot(debugger, &workspace, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_string(*out_view, "selected-scope",
                            snapshot.selected_scope_id);
    }
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view, "scopes",
                             (int64_t)snapshot.visible_scope_count);
    }
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view, "variables",
                             (int64_t)snapshot.visible_variable_count);
    }
    for (index = 0U; status == UMI_STATUS_OK &&
         index < snapshot.visible_variable_count &&
         index < DEBUG_VIEW_ROW_LIMIT; ++index) {
        UmiDebugVariableSnapshot variable;
        char key[32];
        char text[1600];
        status = umi_debug_workspace_variable_at(workspace, index, &variable);
        if (status == UMI_STATUS_OK) {
            (void)snprintf(key, sizeof(key), "variable-%02zu", index + 1U);
            (void)snprintf(text, sizeof(text), "%s%s = %s%s%s",
                           variable.changed ? "● " : "", variable.name,
                           variable.value, variable.type[0] != '\0' ? " : " : "",
                           variable.type);
            status = set_string(*out_view, key, text);
        }
    }
    return status;
}

UmiStatus umi_studio_debug_watches_view_create(
    const char *view_id, UmiStudioDebuggerService *debugger,
    UmiUiViewModel **out_view)
{
    UmiDebugWorkspace *workspace;
    UmiDebugWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "studio.debug-watches", "Watch",
        "Expressions retained across pause and step operations.", out_view);
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    status = workspace_snapshot(debugger, &workspace, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view, "expressions",
                             (int64_t)snapshot.watch_count);
    }
    for (index = 0U; status == UMI_STATUS_OK &&
         index < snapshot.watch_count && index < DEBUG_VIEW_ROW_LIMIT;
         ++index) {
        UmiDebugWatchSnapshot watch;
        char key[32];
        char text[2400];
        status = umi_debug_workspace_watch_at(workspace, index, &watch);
        if (status == UMI_STATUS_OK) {
            (void)snprintf(key, sizeof(key), "watch-%02zu", index + 1U);
            (void)snprintf(text, sizeof(text), "%s = %s%s%s",
                           watch.expression, watch.value,
                           watch.type[0] != '\0' ? " : " : "", watch.type);
            status = set_string(*out_view, key, text);
        }
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 0U, "studio.action.debug.add-watch",
                            "Add Watch…", "Add an expression to the Watch pane");
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 1U, "studio.action.debug.remove-watch",
                            "Remove Watch…", "Remove a watch by its stable ID");
    }
    return status;
}

UmiStatus umi_studio_debug_breakpoints_view_create(
    const char *view_id, UmiStudioDebuggerService *debugger,
    UmiUiViewModel **out_view)
{
    UmiDebugWorkspace *workspace;
    UmiDebugWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "studio.debug-breakpoints", "Breakpoints",
        "Source breakpoints with enabled and adapter verification state.",
        out_view);
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    status = workspace_snapshot(debugger, &workspace, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view, "breakpoints",
                             (int64_t)snapshot.breakpoint_count);
    }
    for (index = 0U; status == UMI_STATUS_OK &&
         index < snapshot.breakpoint_count && index < DEBUG_VIEW_ROW_LIMIT;
         ++index) {
        UmiDebugBreakpointSnapshot breakpoint;
        char key[32];
        char text[1800];
        status = umi_debug_workspace_breakpoint_at(workspace, index,
                                                   &breakpoint);
        if (status == UMI_STATUS_OK) {
            (void)snprintf(key, sizeof(key), "breakpoint-%02zu", index + 1U);
            (void)snprintf(text, sizeof(text), "%s %s:%u%s",
                           breakpoint.enabled ? "●" : "○", breakpoint.uri,
                           breakpoint.line, breakpoint.verified
                           ? " — verified" : " — pending");
            status = set_string(*out_view, key, text);
        }
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 0U,
                            "studio.action.debug.add-breakpoint",
                            "Add Breakpoint…", "Add a source breakpoint");
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 1U,
                            "studio.action.debug.toggle-breakpoint",
                            "Enable or Disable…",
                            "Set breakpoint state using id=enabled");
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 2U,
                            "studio.action.debug.remove-breakpoint",
                            "Remove…", "Remove a breakpoint by its stable ID");
    }
    return status;
}

UmiStatus umi_studio_debug_console_view_create(
    const char *view_id, UmiStudioDebuggerService *debugger,
    UmiUiViewModel **out_view)
{
    UmiDebugWorkspace *workspace;
    UmiDebugWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "studio.debug-console", "Debug Console",
        "Adapter, standard output and evaluation messages from the session.",
        out_view);
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    status = workspace_snapshot(debugger, &workspace, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view, "entries",
                             (int64_t)snapshot.console_entry_count);
    }
    for (index = 0U; status == UMI_STATUS_OK &&
         index < snapshot.console_entry_count && index < DEBUG_VIEW_ROW_LIMIT;
         ++index) {
        UmiDebugConsoleEntrySnapshot entry;
        char key[32];
        char text[2304];
        status = umi_debug_workspace_console_entry_at(workspace, index, &entry);
        if (status == UMI_STATUS_OK) {
            (void)snprintf(key, sizeof(key), "entry-%02zu", index + 1U);
            (void)snprintf(text, sizeof(text), "[%s] %s", entry.category,
                           entry.text);
            status = set_string(*out_view, key, text);
        }
    }
    if (status == UMI_STATUS_OK) {
        status = add_action(*out_view, 0U,
                            "studio.action.debug.clear-console",
                            "Clear", "Clear Debug Console presentation state");
    }
    return status;
}
