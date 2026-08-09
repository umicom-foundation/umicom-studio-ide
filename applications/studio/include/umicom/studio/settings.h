/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/settings.h
 *
 * PURPOSE:
 *   Define the Studio-owned settings schema and provide narrow helpers for
 *   creating, loading, saving and locating the typed Framework settings store.
 *   UI code and feature modules consume this contract rather than parsing JSON
 *   or environment variables directly.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_SETTINGS_H
#define UMICOM_STUDIO_SETTINGS_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_SETTINGS_DEFAULT_PATH "config/studio.settings"

#define UMI_STUDIO_SETTING_UI_THEME "studio.ui.theme"
#define UMI_STUDIO_SETTING_EDITOR_TAB_WIDTH "studio.editor.tab_width"
#define UMI_STUDIO_SETTING_EDITOR_INSERT_SPACES \
    "studio.editor.insert_spaces"
#define UMI_STUDIO_SETTING_WORKSPACE_RESTORE_SESSION \
    "studio.workspace.restore_session"
#define UMI_STUDIO_SETTING_BUILD_PARALLEL_JOBS \
    "studio.build.parallel_jobs"
#define UMI_STUDIO_SETTING_AI_ENABLED "studio.ai.enabled"
#define UMI_STUDIO_SETTING_DIAGNOSTIC_CAPACITY \
    "studio.diagnostics.capacity"
#define UMI_STUDIO_SETTING_AUTO_SAVE "studio.files.auto_save"
#define UMI_STUDIO_SETTING_AUTO_SAVE_SECONDS \
    "studio.files.auto_save_seconds"

UmiStatus umi_studio_settings_create(UmiSettings **out_settings);

UmiStatus umi_studio_settings_load_if_present(
    UmiSettings *settings,
    const char *path,
    int *out_loaded
);

UmiStatus umi_studio_settings_save(
    const UmiSettings *settings,
    const char *path
);

const char *umi_studio_settings_default_path(void);

#ifdef __cplusplus
}
#endif

#endif
