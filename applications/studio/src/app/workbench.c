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
#include "umicom/studio/perspectives.h"
#include "umicom/studio/workbench_shell_catalogue.h"
#include "umicom/studio/workbench_views.h"

#define UMI_STUDIO_WORKBENCH_STATE_SESSION_KEY "studio.ui.workbench-state.v1"
#define UMI_STUDIO_LEGACY_PERSPECTIVE_SESSION_KEY "studio.ui.active-perspective"

UmiStatus umi_studio_workbench_reset_layout(UmiUiWorkbench *workbench)
{
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_studio_contributions_register_layout(workbench);
}

UmiStatus umi_studio_workbench_populate(UmiUiWorkbench *workbench,
                                        UmiStudioServices *services)
{
    UmiStatus status;
    if (workbench == NULL || services == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_studio_perspectives_register(workbench);
    if (status != UMI_STATUS_OK) return status;
    status = umi_studio_contributions_register(workbench);
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
    return umi_ui_workbench_activate_perspective(
        workbench, UMI_STUDIO_DEFAULT_PERSPECTIVE);
}

UmiStatus umi_studio_workbench_restore_session(UmiUiWorkbench *workbench,
                                               UmiSessionStore *session)
{
    char encoded[UMI_UI_WORKBENCH_STATE_TEXT_CAPACITY];
    UmiUiWorkbenchState state;
    UmiStatus status;

    if (workbench == NULL || session == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_session_store_get(session,
                                   UMI_STUDIO_WORKBENCH_STATE_SESSION_KEY,
                                   encoded,
                                   sizeof(encoded));
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

    /* Keep the old key during the migration window for older tooling. */
    status = umi_ui_workbench_snapshot(workbench, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    return umi_session_store_set(session,
                                 UMI_STUDIO_LEGACY_PERSPECTIVE_SESSION_KEY,
                                 snapshot.active_perspective);
}
