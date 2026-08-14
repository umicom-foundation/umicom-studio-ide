/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/coding_assistant.c
 *
 * PURPOSE:
 *   Provide Studio defaults for coding task plans and a narrow repository-root
 *   file adapter for approved Framework patch transactions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Merely preparing a task never reads or edits a file.  The adapter below is
 * passed only to an approved apply/revert call, and every relative path has
 * already passed the Framework's workspace-containment check.
 */
#include "umicom/studio/coding_assistant.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    if (destination == NULL || capacity == 0U || source == NULL) return 0;
    written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

UmiStatus umi_studio_coding_assistant_prepare(
    UmiStudioAiPlatform *platform,
    const char *request_id,
    UmiAiCodingTaskKind task,
    const char *instruction,
    const char *active_path,
    UmiAiCodingTaskPlan *out_plan)
{
    UmiAiCodingRequest request;
    if (platform == NULL || request_id == NULL || instruction == NULL ||
        active_path == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_ai_coding_request_init(&request, task);
    request.context_token_budget =
        umi_studio_ai_platform_coding_context_tokens(platform);
    if (!copy_text(request.request_id, sizeof(request.request_id), request_id) ||
        !copy_text(request.session_id, sizeof(request.session_id),
                   "studio.session.default") ||
        !copy_text(request.runtime_id, sizeof(request.runtime_id),
                   "authorengine.local.chat") ||
        !copy_text(request.workspace_root, sizeof(request.workspace_root), ".") ||
        !copy_text(request.active_path, sizeof(request.active_path), active_path) ||
        !copy_text(request.language_id, sizeof(request.language_id), "c23") ||
        !copy_text(request.instruction, sizeof(request.instruction), instruction)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_ai_coding_assistant_prepare(
        umi_studio_ai_platform_coding_assistant(platform), &request,
        UMI_AI_PROVIDER_AUTHOR_ENGINE, out_plan);
}

UmiStatus umi_studio_coding_assistant_record_patch(
    UmiStudioAiPlatform *platform,
    const UmiAiCodingPatch *patch)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ai_coding_assistant_record_patch(
        umi_studio_ai_platform_coding_assistant(platform), patch);
}

UmiStatus umi_studio_coding_assistant_approve_patch(
    UmiStudioAiPlatform *platform,
    const char *patch_id,
    const char *approved_by)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ai_coding_assistant_approve_patch(
        umi_studio_ai_platform_coding_assistant(platform), patch_id,
        approved_by);
}

static UmiStatus full_path(const UmiStudioCodingWorkspace *workspace,
                           const char *relative_path,
                           char *out_path,
                           size_t capacity)
{
    int written;
    if (workspace == NULL || !umi_ai_coding_path_is_safe_relative(relative_path)) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    written = snprintf(out_path, capacity, "%s/%s", workspace->root,
                       relative_path);
    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

static UmiStatus workspace_read(void *user_data,
                                const char *relative_path,
                                char *out_text,
                                size_t capacity,
                                size_t *out_length)
{
    UmiStudioCodingWorkspace *workspace =
        (UmiStudioCodingWorkspace *)user_data;
    char path[UMI_AI_TEXT_CAPACITY * 2U];
    FILE *file;
    size_t length;
    int extra;
    UmiStatus status = full_path(workspace, relative_path, path, sizeof(path));
    if (status != UMI_STATUS_OK || out_text == NULL || out_length == NULL ||
        capacity < 2U) return status != UMI_STATUS_OK ? status
                                                      : UMI_STATUS_INVALID_ARGUMENT;
    file = fopen(path, "rb");
    if (file == NULL) return errno == ENOENT ? UMI_STATUS_NOT_FOUND
                                             : UMI_STATUS_IO_ERROR;
    length = fread(out_text, 1U, capacity - 1U, file);
    if (ferror(file)) status = UMI_STATUS_IO_ERROR;
    else {
        extra = fgetc(file);
        if (extra != EOF) status = UMI_STATUS_CAPACITY_EXCEEDED;
        else if (ferror(file)) status = UMI_STATUS_IO_ERROR;
    }
    if (status == UMI_STATUS_OK) {
        out_text[length] = '\0';
        *out_length = length;
    }
    (void)fclose(file);
    return status;
}

static UmiStatus workspace_write(void *user_data,
                                 const char *relative_path,
                                 const char *text,
                                 size_t length)
{
    UmiStudioCodingWorkspace *workspace =
        (UmiStudioCodingWorkspace *)user_data;
    char path[UMI_AI_TEXT_CAPACITY * 2U];
    FILE *file;
    UmiStatus status = full_path(workspace, relative_path, path, sizeof(path));
    if (status != UMI_STATUS_OK || text == NULL) return status != UMI_STATUS_OK
        ? status : UMI_STATUS_INVALID_ARGUMENT;
    file = fopen(path, "wb");
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    if (fwrite(text, 1U, length, file) != length || fflush(file) != 0) {
        status = UMI_STATUS_IO_ERROR;
    }
    if (fclose(file) != 0) status = UMI_STATUS_IO_ERROR;
    return status;
}

static UmiStatus workspace_remove(void *user_data, const char *relative_path)
{
    UmiStudioCodingWorkspace *workspace =
        (UmiStudioCodingWorkspace *)user_data;
    char path[UMI_AI_TEXT_CAPACITY * 2U];
    UmiStatus status = full_path(workspace, relative_path, path, sizeof(path));
    if (status != UMI_STATUS_OK) return status;
    return remove(path) == 0 ? UMI_STATUS_OK
                             : (errno == ENOENT ? UMI_STATUS_NOT_FOUND
                                                : UMI_STATUS_IO_ERROR);
}

UmiStatus umi_studio_coding_workspace_adapter_init(
    UmiStudioCodingWorkspace *workspace,
    const char *root,
    UmiAiCodingFileAdapter *out_adapter)
{
    if (workspace == NULL || root == NULL || root[0] == '\0' ||
        out_adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(workspace, 0, sizeof(*workspace));
    (void)memset(out_adapter, 0, sizeof(*out_adapter));
    if (!copy_text(workspace->root, sizeof(workspace->root), root)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    out_adapter->structure_size = (uint32_t)sizeof(*out_adapter);
    out_adapter->abi_version = UMI_AI_CODING_ABI_VERSION;
    out_adapter->read = workspace_read;
    out_adapter->write = workspace_write;
    out_adapter->remove = workspace_remove;
    out_adapter->user_data = workspace;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_coding_assistant_apply_patch(
    UmiStudioAiPlatform *platform,
    const char *patch_id,
    const UmiAiCodingFileAdapter *adapter)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ai_coding_assistant_apply_patch(
        umi_studio_ai_platform_coding_assistant(platform), patch_id, adapter);
}

UmiStatus umi_studio_coding_assistant_revert_patch(
    UmiStudioAiPlatform *platform,
    const char *patch_id,
    const UmiAiCodingFileAdapter *adapter)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ai_coding_assistant_revert_patch(
        umi_studio_ai_platform_coding_assistant(platform), patch_id, adapter);
}
