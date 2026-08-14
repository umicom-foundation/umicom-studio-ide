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
        UMI_STUDIO_SETTING_KNOWLEDGE_ARCHIVE_PATH,
        UMI_SETTING_STRING,
        ".umicom/knowledge-centre.bin",
        "Versioned local archive used by the offline Knowledge Centre.",
        0, 0, 0.0, 0.0, 0, 0
    },
    {
        UMI_STUDIO_SETTING_KNOWLEDGE_SOURCE_CAPACITY,
        UMI_SETTING_INTEGER,
        "1024",
        "Maximum source records retained by the local knowledge catalogue.",
        1, 1000000, 0.0, 0.0, 1, 0
    },
    {
        UMI_STUDIO_SETTING_KNOWLEDGE_VECTOR_CAPACITY,
        UMI_SETTING_INTEGER,
        "8192",
        "Maximum chunk embeddings retained by the active local vector index.",
        1, 10000000, 0.0, 0.0, 1, 0
    },
    {
        UMI_STUDIO_SETTING_KNOWLEDGE_CHUNK_BYTES,
        UMI_SETTING_INTEGER,
        "1200",
        "Target UTF-8 byte window for deterministic source chunking.",
        64, UMI_KNOWLEDGE_TEXT_CAPACITY - 1, 0.0, 0.0, 1, 0
    },
    {
        UMI_STUDIO_SETTING_KNOWLEDGE_OVERLAP_BYTES,
        UMI_SETTING_INTEGER,
        "200",
        "Byte overlap retained between neighbouring knowledge chunks.",
        0, UMI_KNOWLEDGE_TEXT_CAPACITY - 2, 0.0, 0.0, 1, 0
    },
    {
        UMI_STUDIO_SETTING_KNOWLEDGE_RESULT_LIMIT,
        UMI_SETTING_INTEGER,
        "12",
        "Maximum cited matches returned by one Knowledge Centre search.",
        1, UMI_KNOWLEDGE_QUERY_RESULT_MAX, 0.0, 0.0, 1, 0
    },
    {
        UMI_STUDIO_SETTING_KNOWLEDGE_OFFLINE_ONLY,
        UMI_SETTING_BOOLEAN,
        "true",
        "Keep indexing and embedding on configured local providers.",
        0, 0, 0.0, 0.0, 0, 0
    },
    {
        UMI_STUDIO_SETTING_HELIX_MAXIMUM_ATTEMPTS,
        UMI_SETTING_INTEGER,
        "3",
        "Maximum bounded attempts for one Helix v2 build, test or review action.",
        1, 20, 0.0, 0.0, 1, 0
    },
    {
        UMI_STUDIO_SETTING_HELIX_REQUIRE_APPROVAL,
        UMI_SETTING_BOOLEAN,
        "true",
        "Require an exact human approval ticket before Helix v2 executes an action.",
        0, 0, 0.0, 0.0, 0, 0
    },
    {
        UMI_STUDIO_SETTING_HELIX_ALLOW_FILESYSTEM,
        UMI_SETTING_BOOLEAN,
        "true",
        "Allow approved Helix v2 filesystem actions through Studio adapters.",
        0, 0, 0.0, 0.0, 0, 0
    },
    {
        UMI_STUDIO_SETTING_HELIX_ALLOW_BUILD,
        UMI_SETTING_BOOLEAN,
        "true",
        "Allow approved Helix v2 build actions through Studio adapters.",
        0, 0, 0.0, 0.0, 0, 0
    },
    {
        UMI_STUDIO_SETTING_HELIX_ALLOW_SOURCE_CONTROL,
        UMI_SETTING_BOOLEAN,
        "false",
        "Allow separately approved Helix v2 source-control actions.",
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
    },
    {
        UMI_STUDIO_SETTING_VISUAL_BUILDER_GRID_SIZE,
        UMI_SETTING_INTEGER,
        "8",
        "Visual Builder v2 semantic layout grid size in pixels.",
        1, 256, 0.0, 0.0, 1, 0
    },
    {
        UMI_STUDIO_SETTING_VISUAL_BUILDER_SNAP,
        UMI_SETTING_BOOLEAN,
        "true",
        "Snap Visual Builder v2 geometry to the configured grid.",
        0, 0, 0.0, 0.0, 0, 0
    },
    {
        UMI_STUDIO_SETTING_VISUAL_BUILDER_PREVIEW_PROFILE,
        UMI_SETTING_INTEGER,
        "0",
        "Preview profile: 0 desktop through 5 phone portrait.",
        0, 5, 0.0, 0.0, 1, 0
    },
    {
        UMI_STUDIO_SETTING_VISUAL_BUILDER_SHOW_SOURCE,
        UMI_SETTING_BOOLEAN,
        "true",
        "Keep generated declarative, C23 and CMake source visible.",
        0, 0, 0.0, 0.0, 0, 0
    },
    {
        UMI_STUDIO_SETTING_VISUAL_BUILDER_AUTO_PREVIEW,
        UMI_SETTING_BOOLEAN,
        "true",
        "Refresh the live preview after semantic authoring changes.",
        0, 0, 0.0, 0.0, 0, 0
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
