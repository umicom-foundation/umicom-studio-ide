/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/workspace_profiles.c
 *
 * PURPOSE:
 *   Register professional named workspace profiles with the live Framework
 *   workbench used by GTK4, headless tests and future frontend adapters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * These profiles change only visible workbench chrome. They do not delete,
 * move or rewrite project files. Users can switch profiles from the toolbar,
 * the Window menu or the shared command registry, and the active selection is
 * saved in the normal Studio session state.
 */

#include "umicom/studio/workspace_profiles.h"

#include <stdio.h>
#include <string.h>

static UmiStatus register_profile(UmiUiWorkspaceProfileModel *model,
                                  const char *profile_id,
                                  const char *label,
                                  const char *description,
                                  const char *icon_name,
                                  int sidebar_visible,
                                  int auxiliary_visible,
                                  int bottom_visible,
                                  int32_t sidebar_size,
                                  int32_t auxiliary_size,
                                  int32_t bottom_size,
                                  int32_t order)
{
    UmiUiWorkspaceProfileSnapshot profile;
    (void)memset(&profile, 0, sizeof(profile));
    (void)snprintf(profile.profile_id, sizeof(profile.profile_id), "%s",
                   profile_id);
    (void)snprintf(profile.label, sizeof(profile.label), "%s", label);
    (void)snprintf(profile.description, sizeof(profile.description), "%s",
                   description);
    (void)snprintf(profile.icon_name, sizeof(profile.icon_name), "%s",
                   icon_name);
    profile.sidebar_visible = sidebar_visible;
    profile.auxiliary_sidebar_visible = auxiliary_visible;
    profile.bottom_panel_visible = bottom_visible;
    profile.sidebar_size = sidebar_size;
    profile.auxiliary_sidebar_size = auxiliary_size;
    profile.bottom_panel_size = bottom_size;
    profile.order = order;
    profile.built_in = 1;
    return umi_ui_workspace_profile_model_upsert(model, &profile);
}

UmiStatus umi_studio_workspace_profiles_register(UmiUiWorkbench *workbench)
{
    UmiUiWorkspaceProfileModel *model;
    UmiStatus status;

    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model = umi_ui_workbench_workspace_profiles(workbench);

    status = register_profile(
        model, UMI_STUDIO_WORKSPACE_PROFILE_DEVELOP, "Develop",
        "Editor-first coding with project tools and a compact bottom panel",
        "applications-development-symbolic", 1, 0, 1, 288, 360, 240, 10);
    if (status != UMI_STATUS_OK) return status;
    status = register_profile(
        model, UMI_STUDIO_WORKSPACE_PROFILE_FOCUS, "Focus",
        "Distraction-free editor with all tool regions temporarily hidden",
        "view-fullscreen-symbolic", 0, 0, 0, 288, 360, 240, 20);
    if (status != UMI_STATUS_OK) return status;
    status = register_profile(
        model, UMI_STUDIO_WORKSPACE_PROFILE_DEBUG, "Debug",
        "Source, variables, call stack and an expanded debugging console",
        "applications-engineering-symbolic", 1, 1, 1, 300, 380, 300, 30);
    if (status != UMI_STATUS_OK) return status;
    status = register_profile(
        model, UMI_STUDIO_WORKSPACE_PROFILE_REVIEW, "Review",
        "Code review and comparison with navigation and auxiliary context",
        "document-properties-symbolic", 1, 1, 1, 260, 420, 260, 40);
    if (status != UMI_STATUS_OK) return status;
    status = register_profile(
        model, UMI_STUDIO_WORKSPACE_PROFILE_OPERATIONS, "Operations",
        "Monitoring, diagnostics, tasks and operational evidence",
        "utilities-system-monitor-symbolic", 1, 1, 1, 280, 420, 320, 50);
    if (status != UMI_STATUS_OK) return status;
    status = register_profile(
        model, UMI_STUDIO_WORKSPACE_PROFILE_TRADING, "Trading",
        "TWS-inspired market, chart, order, risk and activity workspace",
        "view-statistics-symbolic", 1, 1, 1, 260, 390, 300, 60);
    if (status != UMI_STATUS_OK) return status;

    return umi_ui_workbench_activate_workspace_profile(
        workbench, UMI_STUDIO_WORKSPACE_PROFILE_DEVELOP);
}
