/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/appearance_centre.c
 *
 * PURPOSE:
 *   Register the professional Umicom appearance family, keep the active theme
 *   reflected in workbench context, and persist the user's selection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The trademark artwork remains an application asset.  Framework stores only
 * the resource locations and semantic presentation values, so another Umicom
 * application can provide its own brand while reusing the same theme engine.
 */

#include "umicom/studio/appearance_centre.h"

#include <stdio.h>
#include <string.h>

static const char *density_name(UmiUiDensity density)
{
    switch (density) {
        case UMI_UI_DENSITY_COMPACT: return "compact";
        case UMI_UI_DENSITY_COMFORTABLE: return "comfortable";
        case UMI_UI_DENSITY_SPACIOUS: return "spacious";
        default: return "comfortable";
    }
}

static UmiStatus sync_context(UmiUiWorkbench *workbench,
                              const UmiUiAppearanceProfile *profile)
{
    UmiUiContextStore *context = umi_ui_workbench_context(workbench);
    UmiStatus status;
    if (context == NULL || profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ui_context_set_string(context, "studio.ui.theme",
                                       profile->profile_id);
    if (status == UMI_STATUS_OK) {
        status = umi_ui_context_set_string(context, "studio.ui.density",
                                           density_name(profile->density));
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_context_set_string(context, "studio.ui.font.interface",
                                           profile->interface_font);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_context_set_string(context, "studio.ui.font.editor",
                                           profile->editor_font);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_context_set_integer(
            context, "studio.ui.font.scale-per-thousand",
            (int64_t)(profile->font_scale * 1000.0));
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_context_set_boolean(context, "studio.ui.reduce-motion",
                                            profile->reduce_motion);
    }
    return status;
}

static UmiStatus add_builtin(UmiUiWorkbench *workbench,
                             const char *profile_id,
                             const char *label,
                             const char *description,
                             UmiUiThemeMode mode,
                             UmiUiDensity density,
                             int active)
{
    UmiUiAppearanceProfile profile;
    UmiStatus status = umi_ui_appearance_profile_init(
        &profile, profile_id, label, mode, density);
    if (status != UMI_STATUS_OK) return status;

    (void)snprintf(profile.description, sizeof(profile.description), "%s",
                   description);
    (void)snprintf(profile.logo_resource, sizeof(profile.logo_resource), "%s",
                   "branding/umicom-logo.svg");
    (void)snprintf(profile.icon_resource, sizeof(profile.icon_resource), "%s",
                   "branding/umicom-icon.svg");
    profile.active = active != 0;
    profile.built_in = 1;
    profile.locked = 1;
    return umi_ui_appearance_model_upsert(
        umi_ui_workbench_appearance(workbench), &profile);
}

UmiStatus umi_studio_appearance_register(UmiUiWorkbench *workbench)
{
    UmiStatus status;
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = add_builtin(
        workbench, UMI_STUDIO_APPEARANCE_DARK, "Umicom Dark",
        "Trademark navy workspace with a restrained red action accent",
        UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMPACT, 1);
    if (status == UMI_STATUS_OK) {
        status = add_builtin(
            workbench, UMI_STUDIO_APPEARANCE_LIGHT, "Umicom Light",
            "Bright professional workspace using Umicom navy typography",
            UMI_UI_THEME_MODE_LIGHT, UMI_UI_DENSITY_COMFORTABLE, 0);
    }
    if (status == UMI_STATUS_OK) {
        status = add_builtin(
            workbench, UMI_STUDIO_APPEARANCE_SYSTEM, "Follow System",
            "Use the operating-system preference with the Umicom brand palette",
            UMI_UI_THEME_MODE_SYSTEM, UMI_UI_DENSITY_COMFORTABLE, 0);
    }
    if (status == UMI_STATUS_OK) {
        status = add_builtin(
            workbench, UMI_STUDIO_APPEARANCE_HIGH_CONTRAST,
            "Umicom High Contrast",
            "Maximum separation, strong focus indicators and reduced motion",
            UMI_UI_THEME_MODE_HIGH_CONTRAST, UMI_UI_DENSITY_SPACIOUS, 0);
    }
    if (status != UMI_STATUS_OK) return status;
    return umi_studio_appearance_activate(workbench,
                                          UMI_STUDIO_APPEARANCE_DARK);
}

UmiStatus umi_studio_appearance_activate(UmiUiWorkbench *workbench,
                                         const char *profile_id)
{
    UmiUiAppearanceProfile profile;
    UmiUiAppearanceModel *model;
    UmiStatus status;
    if (workbench == NULL || profile_id == NULL || profile_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    model = umi_ui_workbench_appearance(workbench);
    status = umi_ui_appearance_model_set_active(model, profile_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_appearance_model_active(model, &profile);
    if (status != UMI_STATUS_OK) return status;
    return sync_context(workbench, &profile);
}

UmiStatus umi_studio_appearance_restore(UmiUiWorkbench *workbench,
                                        UmiSessionStore *session)
{
    char profile_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    UmiStatus status;
    if (workbench == NULL || session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_session_store_get(session, UMI_STUDIO_APPEARANCE_SESSION_KEY,
                                   profile_id, sizeof(profile_id));
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;
    status = umi_studio_appearance_activate(workbench, profile_id);
    /* A deleted extension-owned custom profile must not prevent Studio from
     * starting.  The protected Umicom Dark profile is the safe fallback. */
    if (status == UMI_STATUS_NOT_FOUND) {
        return umi_studio_appearance_activate(
            workbench, UMI_STUDIO_APPEARANCE_DARK);
    }
    return status;
}

UmiStatus umi_studio_appearance_save(UmiUiWorkbench *workbench,
                                     UmiSessionStore *session)
{
    UmiUiAppearanceProfile profile;
    UmiStatus status;
    if (workbench == NULL || session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_appearance_model_active(
        umi_ui_workbench_appearance(workbench), &profile);
    if (status != UMI_STATUS_OK) return status;
    return umi_session_store_set(session, UMI_STUDIO_APPEARANCE_SESSION_KEY,
                                 profile.profile_id);
}

UmiStatus umi_studio_appearance_set_brand_resources(
    UmiUiWorkbench *workbench,
    const char *logo_path,
    const char *icon_path)
{
    UmiUiContextStore *context;
    UmiStatus status;
    if (workbench == NULL || logo_path == NULL || icon_path == NULL ||
        logo_path[0] == '\0' || icon_path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    context = umi_ui_workbench_context(workbench);
    status = umi_ui_context_set_string(context, "studio.brand.name", "Umicom");
    if (status == UMI_STATUS_OK) {
        status = umi_ui_context_set_string(context, "studio.brand.logo-path",
                                           logo_path);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_context_set_string(context, "studio.brand.icon-path",
                                           icon_path);
    }
    return status;
}
