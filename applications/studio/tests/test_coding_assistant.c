/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_coding_assistant.c
 *
 * PURPOSE:
 *   Verify Studio composes Framework coding task plans and approved patch
 *   apply/revert operations through its repository-root file adapter.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/studio/coding_assistant.h"

int main(void)
{
    UmiStudioAiPlatform *platform = NULL;
    UmiAiCodingTaskPlan plan;
    UmiAiCodingPatch patch;
    UmiStudioCodingWorkspace workspace;
    UmiAiCodingFileAdapter adapter;
    FILE *file;
    char text[64] = {0};
    const char *path = "umicom-studio-coding-test.c";

    assert(umi_studio_ai_platform_create(&platform) == UMI_STATUS_OK);
    assert(umi_studio_ai_platform_coding_assistant(platform) != NULL);
    assert(umi_studio_coding_assistant_prepare(
        platform, "studio.request.48", UMI_AI_CODING_TASK_EXPLAIN,
        "Explain the AI platform ownership boundary.",
        "applications/studio/src/app/ai_platform.c", &plan)
        == UMI_STATUS_OK);
    assert(plan.ready && plan.repository_context.file_count > 0U);

    assert(umi_ai_coding_patch_init(
        &patch, "studio.patch.48", "studio.request.48", "Create test file",
        "Exercise controlled creation and reversion") == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_add_file(
        &patch, path, UMI_AI_CODING_PATCH_CREATE, "", "int value = 48;\n")
        == UMI_STATUS_OK);
    assert(umi_studio_coding_assistant_record_patch(platform, &patch)
           == UMI_STATUS_OK);
    assert(umi_studio_coding_assistant_approve_patch(
        platform, patch.patch_id, "test.user") == UMI_STATUS_OK);
    assert(umi_studio_coding_workspace_adapter_init(
        &workspace, ".", &adapter) == UMI_STATUS_OK);
    assert(umi_studio_coding_assistant_apply_patch(
        platform, patch.patch_id, &adapter) == UMI_STATUS_OK);
    file = fopen(path, "rb");
    assert(file != NULL);
    assert(fread(text, 1U, sizeof(text) - 1U, file) > 0U);
    (void)fclose(file);
    assert(strcmp(text, "int value = 48;\n") == 0);
    assert(umi_studio_coding_assistant_revert_patch(
        platform, patch.patch_id, &adapter) == UMI_STATUS_OK);
    assert(fopen(path, "rb") == NULL);
    umi_studio_ai_platform_destroy(platform);
    return 0;
}
