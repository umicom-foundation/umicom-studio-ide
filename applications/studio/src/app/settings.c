/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/settings.c
 *
 * PURPOSE:
 *   Implement the Studio settings schema on top of Umicom Framework's typed
 *   settings repository.  Defaults and ranges are declared once here so the
 *   console, GTK4 frontend, tests and future plug-ins share one definition.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/settings.h"

static const UmiSettingDefinition STUDIO_SETTINGS_SCHEMA[] = {
    {
        UMI_STUDIO_SETTING_UI_THEME,
        UMI_SETTING_STRING,
        "system",
        "GTK4 theme preference: system, light or dark.",
        0,
        0,
        0.0,
        0.0,
        0,
        0
    },
    {
        UMI_STUDIO_SETTING_EDITOR_TAB_WIDTH,
        UMI_SETTING_INTEGER,
        "4",
        "Editor tab width in columns.",
        1,
        16,
        0.0,
        0.0,
        1,
        0
    },
    {
        UMI_STUDIO_SETTING_EDITOR_INSERT_SPACES,
        UMI_SETTING_BOOLEAN,
        "true",
        "Insert spaces instead of a literal tab character.",
        0,
        0,
        0.0,
        0.0,
        0,
        0
    },
    {
        UMI_STUDIO_SETTING_WORKSPACE_RESTORE_SESSION,
        UMI_SETTING_BOOLEAN,
        "true",
        "Restore the previous workspace and open documents at startup.",
        0,
        0,
        0.0,
        0.0,
        0,
        0
    },
    {
        UMI_STUDIO_SETTING_BUILD_PARALLEL_JOBS,
        UMI_SETTING_INTEGER,
        "4",
        "Maximum parallel build jobs requested by Studio.",
        1,
        64,
        0.0,
        0.0,
        1,
        0
    },
    {
        UMI_STUDIO_SETTING_AI_ENABLED,
        UMI_SETTING_BOOLEAN,
        "true",
        "Enable the local or remote AI provider gateway in Studio.",
        0,
        0,
        0.0,
        0.0,
        0,
        0
    },
    {
        UMI_STUDIO_SETTING_AUTHORENGINE_EXECUTABLE,
        UMI_SETTING_STRING,
        "uaengine",
        "AuthorEngine executable used by the supervised process adapter.",
        0, 0, 0.0, 0.0, 0, 0
    },
    {
        UMI_STUDIO_SETTING_AUTHORENGINE_WORKSPACE,
        UMI_SETTING_STRING,
        ".",
        "Workspace supplied to AuthorEngine; dot selects Studio's current workspace.",
        0, 0, 0.0, 0.0, 0, 0
    },
    {
        UMI_STUDIO_SETTING_AI_CONTEXT_TOKENS,
        UMI_SETTING_INTEGER,
        "32768",
        "Maximum combined input and reserved output tokens for one AI request.",
        2048, 1048576, 0.0, 0.0, 1, 0
    },
    {
        UMI_STUDIO_SETTING_AI_RESERVED_OUTPUT_TOKENS,
        UMI_SETTING_INTEGER,
        "2048",
        "Tokens reserved for the model response before selecting workspace context.",
        256, 262144, 0.0, 0.0, 1, 0
    },
    {
        UMI_STUDIO_SETTING_AI_ALLOW_REMOTE,
        UMI_SETTING_BOOLEAN,
        "false",
        "Permit configured remote AI runtimes; sensitive sharing still requires policy approval.",
        0, 0, 0.0, 0.0, 0, 0
    },
    {
        UMI_STUDIO_SETTING_AI_PERSIST_SESSIONS,
        UMI_SETTING_BOOLEAN,
        "true",
        "Permit explicitly requested AI session persistence under privacy policy.",
        0, 0, 0.0, 0.0, 0, 0
    },
    {
        UMI_STUDIO_SETTING_AI_CODING_CONTEXT_TOKENS,
        UMI_SETTING_INTEGER,
        "16384",
        "Maximum repository-file tokens selected for one coding task.",
        1024, 524288, 0.0, 0.0, 1, 0
    },
    {
        UMI_STUDIO_SETTING_AI_CODING_MAX_PATCH_FILES,
        UMI_SETTING_INTEGER,
        "16",
        "Maximum files accepted in one AI coding patch preview.",
        1, UMI_AI_CODING_PATCH_FILE_MAX, 0.0, 0.0, 1, 0
    },
    {
        UMI_STUDIO_SETTING_AI_CODING_MAX_PATCH_LINES,
        UMI_SETTING_INTEGER,
        "2000",
        "Maximum total added and removed lines in one AI coding patch.",
        1, 100000, 0.0, 0.0, 1, 0
    },
    {
        UMI_STUDIO_SETTING_AI_CODING_ALLOW_CREATE,
        UMI_SETTING_BOOLEAN,
        "true",
        "Allow reviewed AI coding patches to create files.",
        0, 0, 0.0, 0.0, 0, 0
    },
    {
        UMI_STUDIO_SETTING_AI_CODING_ALLOW_DELETE,
        UMI_SETTING_BOOLEAN,
        "false",
        "Allow reviewed AI coding patches to delete files.",
        0, 0, 0.0, 0.0, 0, 0
    },
    {
        UMI_STUDIO_SETTING_AI_CODING_REQUIRE_APPROVAL,
        UMI_SETTING_BOOLEAN,
        "true",
        "Require explicit user approval before an AI coding patch is applied.",
        0, 0, 0.0, 0.0, 0, 0
    },
    {
        UMI_STUDIO_SETTING_DIAGNOSTIC_CAPACITY,
        UMI_SETTING_INTEGER,
        "512",
        "Maximum retained Framework diagnostic records.",
        64,
        65536,
        0.0,
        0.0,
        1,
        0
    },
    {
        UMI_STUDIO_SETTING_AUTO_SAVE,
        UMI_SETTING_BOOLEAN,
        "false",
        "Enable periodic saving of modified editor documents.",
        0,
        0,
        0.0,
        0.0,
        0,
        0
    },
    {
        UMI_STUDIO_SETTING_AUTO_SAVE_SECONDS,
        UMI_SETTING_INTEGER,
        "60",
        "Auto-save interval in seconds when auto-save is enabled.",
        5,
        3600,
        0.0,
        0.0,
        1,
        0
    }
};

UmiStatus umi_studio_settings_create(UmiSettings **out_settings)
{
    return umi_settings_create(
        STUDIO_SETTINGS_SCHEMA,
        sizeof(STUDIO_SETTINGS_SCHEMA) / sizeof(STUDIO_SETTINGS_SCHEMA[0]),
        out_settings
    );
}

UmiStatus umi_studio_settings_load_if_present(UmiSettings *settings,
                                              const char *path,
                                              int *out_loaded)
{
    if (settings == NULL || path == NULL || out_loaded == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_loaded = 0;
    if (umi_fs_exists(path) == 0) {
        return UMI_STATUS_OK;
    }

    if (umi_settings_load_file(settings, path) != UMI_STATUS_OK) {
        return UMI_STATUS_PARSE_ERROR;
    }

    *out_loaded = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_settings_save(const UmiSettings *settings,
                                   const char *path)
{
    return umi_settings_save_file(settings, path);
}

const char *umi_studio_settings_default_path(void)
{
    return UMI_STUDIO_SETTINGS_DEFAULT_PATH;
}
