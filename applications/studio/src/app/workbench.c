/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/workbench.c
 *
 * PURPOSE:
 *   Compose the default Studio workbench from Framework perspectives, panes,
 *   actions, menus, toolbars, status items and serialisable layout state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/workbench.h"

#include <stdio.h>
#include <string.h>

#include "umicom/studio/contributions.h"
#include "umicom/studio/perspectives.h"

UmiStatus umi_studio_workbench_reset_layout(UmiUiWorkbench *workbench)
{
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_studio_contributions_register_layout(workbench);
}

UmiStatus umi_studio_workbench_populate(UmiUiWorkbench *workbench,
                                        UmiStudioServices *services)
{
    UmiStatus status;
    (void)services;
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_studio_perspectives_register(workbench);
    if (status != UMI_STATUS_OK) return status;
    status = umi_studio_contributions_register(workbench);
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_workbench_activate_perspective(workbench,
                                                  UMI_STUDIO_DEFAULT_PERSPECTIVE);
}

UmiStatus umi_studio_workbench_restore_session(UmiUiWorkbench *workbench,
                                               UmiSessionStore *session)
{
    char perspective[UMI_UI_ID_CAPACITY];
    UmiStatus status;
    if (workbench == NULL || session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_session_store_get(session, "studio.ui.active-perspective",
                                   perspective, sizeof(perspective));
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;
    if (perspective[0] != '\0') {
        status = umi_ui_workbench_activate_perspective(workbench, perspective);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_workbench_save_session(UmiUiWorkbench *workbench,
                                            UmiSessionStore *session)
{
    UmiUiWorkbenchSnapshot snapshot;
    UmiStatus status;
    if (workbench == NULL || session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_workbench_snapshot(workbench, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    return umi_session_store_set(session, "studio.ui.active-perspective",
                                 snapshot.active_perspective);
}
