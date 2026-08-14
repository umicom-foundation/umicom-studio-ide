/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/coding_assistant.h
 *
 * PURPOSE:
 *   Compose Framework AI coding assistance into Studio task, patch and
 *   workspace-file workflows without taking over AuthorEngine orchestration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio supplies product defaults and a workspace file adapter.  Repository
 * planning, approval, conflict detection and patch rollback remain reusable
 * Framework responsibilities exposed through the stable C ABI.
 */
#ifndef UMICOM_STUDIO_CODING_ASSISTANT_H
#define UMICOM_STUDIO_CODING_ASSISTANT_H

#include "umicom/studio/ai_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioCodingWorkspace {
    char root[UMI_AI_TEXT_CAPACITY];
} UmiStudioCodingWorkspace;

UmiStatus umi_studio_coding_assistant_prepare(
    UmiStudioAiPlatform *platform,
    const char *request_id,
    UmiAiCodingTaskKind task,
    const char *instruction,
    const char *active_path,
    UmiAiCodingTaskPlan *out_plan);
UmiStatus umi_studio_coding_assistant_record_patch(
    UmiStudioAiPlatform *platform,
    const UmiAiCodingPatch *patch);
UmiStatus umi_studio_coding_assistant_approve_patch(
    UmiStudioAiPlatform *platform,
    const char *patch_id,
    const char *approved_by);
UmiStatus umi_studio_coding_workspace_adapter_init(
    UmiStudioCodingWorkspace *workspace,
    const char *root,
    UmiAiCodingFileAdapter *out_adapter);
UmiStatus umi_studio_coding_assistant_apply_patch(
    UmiStudioAiPlatform *platform,
    const char *patch_id,
    const UmiAiCodingFileAdapter *adapter);
UmiStatus umi_studio_coding_assistant_revert_patch(
    UmiStudioAiPlatform *platform,
    const char *patch_id,
    const UmiAiCodingFileAdapter *adapter);

#ifdef __cplusplus
}
#endif

#endif
