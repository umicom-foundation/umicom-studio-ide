/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/workspace_profile_session.c
 *
 * PURPOSE:
 *   Save and restore custom named layouts, including draggable tool-window
 *   placements, through the normal Studio session lifecycle.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The manifest lists only user-created profile IDs. Built-in profiles remain
 * source-controlled Studio defaults and are never overwritten by session data.
 * Old unused values may remain in the session file, but are ignored unless
 * listed by the current manifest; no destructive cleanup is performed.
 */

#include "umicom/studio/workspace_profile_session.h"

#include <stdio.h>
#include <string.h>

#include "umicom/ui/workspace_profile_persistence.h"

#define UMI_STUDIO_WORKSPACE_PROFILE_MANIFEST_KEY \
    "studio.ui.workspace-profiles.v1"
#define UMI_STUDIO_WORKSPACE_PROFILE_KEY_PREFIX \
    "studio.ui.workspace-profile."

static UmiStatus append_manifest_id(char *manifest,
                                    size_t capacity,
                                    const char *profile_id)
{
    size_t length = strlen(manifest);
    size_t id_length = strlen(profile_id);
    size_t separator = length == 0U ? 0U : 1U;
    if (length + separator + id_length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (separator != 0U) manifest[length++] = '|';
    (void)memcpy(manifest + length, profile_id, id_length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_workspace_profile_session_save(
    UmiUiWorkbench *workbench,
    UmiSessionStore *session)
{
    UmiUiWorkspaceProfileModel *model;
    char manifest[UMI_SESSION_VALUE_CAPACITY] = "";
    size_t index;
    UmiStatus status;

    if (workbench == NULL || session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    model = umi_ui_workbench_workspace_profiles(workbench);
    for (index = 0U; index < umi_ui_workspace_profile_model_count(model);
         ++index) {
        UmiUiWorkspaceProfileSnapshot profile;
        char encoded[UMI_UI_WORKSPACE_PROFILE_TEXT_CAPACITY];
        char key[UMI_SESSION_KEY_CAPACITY];
        int written;

        status = umi_ui_workspace_profile_model_at(model, index, &profile);
        if (status != UMI_STATUS_OK) return status;
        if (profile.built_in) continue;
        status = umi_ui_workspace_profile_encode(
            &profile, encoded, sizeof(encoded));
        if (status != UMI_STATUS_OK) return status;
        written = snprintf(key, sizeof(key), "%s%s",
                           UMI_STUDIO_WORKSPACE_PROFILE_KEY_PREFIX,
                           profile.profile_id);
        if (written < 0 || (size_t)written >= sizeof(key)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = umi_session_store_set(session, key, encoded);
        if (status != UMI_STATUS_OK) return status;
        status = append_manifest_id(manifest,
                                    sizeof(manifest),
                                    profile.profile_id);
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_session_store_set(
        session, UMI_STUDIO_WORKSPACE_PROFILE_MANIFEST_KEY, manifest);
}

UmiStatus umi_studio_workspace_profile_session_restore(
    UmiUiWorkbench *workbench,
    UmiSessionStore *session)
{
    char manifest[UMI_SESSION_VALUE_CAPACITY];
    char *cursor;
    UmiStatus status;

    if (workbench == NULL || session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_session_store_get(
        session,
        UMI_STUDIO_WORKSPACE_PROFILE_MANIFEST_KEY,
        manifest,
        sizeof(manifest));
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;

    cursor = manifest;
    while (cursor[0] != '\0') {
        char *separator = strchr(cursor, '|');
        char key[UMI_SESSION_KEY_CAPACITY];
        char encoded[UMI_SESSION_VALUE_CAPACITY];
        UmiUiWorkspaceProfileSnapshot profile;
        int written;
        if (separator != NULL) *separator = '\0';
        if (strncmp(cursor, "custom-", strlen("custom-")) != 0) {
            return UMI_STATUS_PARSE_ERROR;
        }
        written = snprintf(key, sizeof(key), "%s%s",
                           UMI_STUDIO_WORKSPACE_PROFILE_KEY_PREFIX,
                           cursor);
        if (written < 0 || (size_t)written >= sizeof(key)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = umi_session_store_get(
            session, key, encoded, sizeof(encoded));
        if (status != UMI_STATUS_OK) return status;
        status = umi_ui_workspace_profile_decode(encoded, &profile);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(profile.profile_id, cursor) != 0 || profile.built_in) {
            return UMI_STATUS_PARSE_ERROR;
        }
        profile.active = 0;
        status = umi_ui_workspace_profile_model_upsert(
            umi_ui_workbench_workspace_profiles(workbench), &profile);
        if (status != UMI_STATUS_OK) return status;
        if (separator == NULL) break;
        cursor = separator + 1;
    }
    return UMI_STATUS_OK;
}
