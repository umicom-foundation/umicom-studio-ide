/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/workbench.c
 *
 * PURPOSE:
 *   Compose the Studio workbench from Framework perspectives, panes, Activity
 *   Bar destinations, view containers, keybindings and serialisable state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio chooses which Framework capabilities it wants. Framework owns the
 * behaviour. This keeps the same workbench state usable by GTK4 and headless
 * tests and leaves future Qt/Wt adapters possible.
 */

#include "umicom/studio/workbench.h"

#include <stdio.h>
#include <string.h>

#include "umicom/studio/contributions.h"
#include "umicom/studio/appearance_centre.h"
#include "umicom/studio/editor_layout_session.h"
#include "umicom/studio/perspectives.h"
#include "umicom/studio/workbench_shell_catalogue.h"
#include "umicom/studio/workbench_views.h"
#include "umicom/studio/workspace_profile_session.h"
#include "umicom/studio/workspace_profiles.h"

#define UMI_STUDIO_WORKBENCH_STATE_SESSION_KEY "studio.ui.workbench-state.v2"
#define UMI_STUDIO_LEGACY_WORKBENCH_STATE_SESSION_KEY \
    "studio.ui.workbench-state.v1"
#define UMI_STUDIO_LEGACY_PERSPECTIVE_SESSION_KEY "studio.ui.active-perspective"

UmiStatus umi_studio_workbench_reset_layout(UmiUiWorkbench *workbench)
{
    UmiStatus status;
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_studio_contributions_register_layout(workbench);
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_workbench_activate_workspace_profile(
        workbench, UMI_STUDIO_WORKSPACE_PROFILE_DEVELOP);
}

UmiStatus umi_studio_workbench_populate(UmiUiWorkbench *workbench,
                                        UmiStudioServices *services)
{
    static const char WELCOME_SOURCE[] =
        "/* Umicom Studio IDE — Framework Workbench */\n"
        "\n"
        "#include <stdio.h>\n"
        "#include \"umicom/umicom.h\"\n"
        "\n"
        "int main(void)\n"
        "{\n"
        "    puts(\"Welcome to Umicom Studio\");\n"
        "    puts(\"Framework owns reusable editor and workbench services.\");\n"
        "    return 0;\n"
        "}\n";
    UmiUiDocumentViewSnapshot welcome = {0};
    UmiStatus status;
    if (workbench == NULL || services == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_studio_perspectives_register(workbench);
    if (status != UMI_STATUS_OK) return status;
    status = umi_studio_contributions_register(workbench);
    if (status != UMI_STATUS_OK) return status;
    status = umi_studio_workspace_profiles_register(workbench);
    if (status != UMI_STATUS_OK) return status;
    status = umi_studio_appearance_register(workbench);
    if (status != UMI_STATUS_OK) return status;

    /*
     * Register product view factories before the Activity Bar is activated.
     * GTK4 and future adapters can then materialise real Studio data when the
     * selected view container becomes visible.
     */
    status = umi_studio_workbench_views_register(workbench, services);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_workbench_shell_catalogue_register(workbench, services);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workbench_activate_perspective(
        workbench, UMI_STUDIO_DEFAULT_PERSPECTIVE);
    if (status != UMI_STATUS_OK) return status;

    /*
     * Present a real editable document on first launch. Product-neutral tab
     * state and content transport live in Framework; Studio supplies only the
     * welcome document identity and text.
     */
    (void)snprintf(welcome.view_id, sizeof(welcome.view_id), "%s",
                   "studio.editor.welcome");
    (void)snprintf(welcome.document_id, sizeof(welcome.document_id), "%s",
                   "studio.document.welcome");
    (void)snprintf(welcome.title, sizeof(welcome.title), "%s", "Welcome.c");
    (void)snprintf(welcome.icon_name, sizeof(welcome.icon_name), "%s",
                   "text-x-csrc-symbolic");
    (void)snprintf(welcome.uri, sizeof(welcome.uri), "%s",
                   "umicom://studio/welcome.c");
    (void)snprintf(welcome.language_id, sizeof(welcome.language_id), "%s", "c");
    (void)snprintf(welcome.group_id, sizeof(welcome.group_id), "%s",
                   "editor.primary");
    (void)snprintf(welcome.source_text, sizeof(welcome.source_text), "%s",
                   WELCOME_SOURCE);
    welcome.active = 1;
    welcome.pinned = 1;
    welcome.closable = 1;
    welcome.show_line_numbers = 1;
    status = umi_ui_document_view_model_upsert(
        umi_ui_workbench_documents(workbench), &welcome);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workbench_activate_document(workbench, welcome.view_id);
    if (status != UMI_STATUS_OK) return status;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_workbench_restore_session(UmiUiWorkbench *workbench,
                                               UmiSessionStore *session)
{
    char encoded[UMI_UI_WORKBENCH_STATE_TEXT_CAPACITY];
    UmiUiDocumentLayoutApplyResult editor_layout_result;
    UmiUiWorkbenchState state;
    UmiStatus status;

    if (workbench == NULL || session == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Custom profiles must exist before the general workbench-state decoder
     * tries to reactivate one by ID. Built-in defaults are already registered
     * during workbench population. */
    status = umi_studio_workspace_profile_session_restore(workbench, session);
    if (status != UMI_STATUS_OK) return status;
    status = umi_studio_appearance_restore(workbench, session);
    if (status != UMI_STATUS_OK) return status;
    status = umi_studio_editor_layout_session_restore(
        workbench, session, &editor_layout_result);
    if (status != UMI_STATUS_OK) return status;

    status = umi_session_store_get(session,
                                   UMI_STUDIO_WORKBENCH_STATE_SESSION_KEY,
                                   encoded,
                                   sizeof(encoded));
    if (status == UMI_STATUS_NOT_FOUND) {
        /* Read the Batch 23 key once; the next save migrates it to Version 2. */
        status = umi_session_store_get(
            session,
            UMI_STUDIO_LEGACY_WORKBENCH_STATE_SESSION_KEY,
            encoded,
            sizeof(encoded));
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workbench_state_decode(encoded, &state);
        if (status != UMI_STATUS_OK) return status;
        return umi_ui_workbench_state_apply(workbench, &state);
    }
    if (status != UMI_STATUS_NOT_FOUND) return status;

    /*
     * Compatibility with sessions created before Batch 23. Once restored, the
     * next normal shutdown writes the richer Framework workbench state.
     */
    {
        char perspective[UMI_UI_ID_CAPACITY];
        status = umi_session_store_get(session,
                                       UMI_STUDIO_LEGACY_PERSPECTIVE_SESSION_KEY,
                                       perspective,
                                       sizeof(perspective));
        if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
        if (status != UMI_STATUS_OK) return status;
        if (perspective[0] == '\0') return UMI_STATUS_OK;
        return umi_ui_workbench_activate_perspective(workbench, perspective);
    }
}

UmiStatus umi_studio_workbench_save_session(UmiUiWorkbench *workbench,
                                            UmiSessionStore *session)
{
    UmiUiWorkbenchState state;
    UmiUiWorkbenchSnapshot snapshot;
    char encoded[UMI_UI_WORKBENCH_STATE_TEXT_CAPACITY];
    UmiStatus status;

    if (workbench == NULL || session == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ui_workbench_state_snapshot(workbench, &state);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workbench_state_encode(&state, encoded, sizeof(encoded));
    if (status != UMI_STATUS_OK) return status;
    status = umi_session_store_set(session,
                                   UMI_STUDIO_WORKBENCH_STATE_SESSION_KEY,
                                   encoded);
    if (status != UMI_STATUS_OK) return status;
    status = umi_studio_workspace_profile_session_save(workbench, session);
    if (status != UMI_STATUS_OK) return status;
    status = umi_studio_appearance_save(workbench, session);
    if (status != UMI_STATUS_OK) return status;
    status = umi_studio_editor_layout_session_save(workbench, session);
    if (status != UMI_STATUS_OK) return status;

    /* Keep the old key during the migration window for older tooling. */
    status = umi_ui_workbench_snapshot(workbench, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    return umi_session_store_set(session,
                                 UMI_STUDIO_LEGACY_PERSPECTIVE_SESSION_KEY,
                                 snapshot.active_perspective);
}
