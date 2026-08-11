/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/helix.c
 *
 * PURPOSE:
 *   Report and advance Studio's shared Helix workflow.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The wrapper keeps workflow stage changes behind one Studio use case that can later add audit and permissions.
 */

#include "umicom/studio/helix.h"

#include <stdio.h>

UmiStatus umi_studio_helix_status(UmiStudioAiPlatform *platform,
                                  char *output,
                                  size_t output_capacity)
{
    UmiHelixRuntime *runtime;
    int written;
    if (platform == NULL || output == NULL || output_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    runtime = umi_studio_ai_platform_helix(platform);
    if (runtime == NULL) return UMI_STATUS_INVALID_STATE;
    written = snprintf(output, output_capacity,
                       "workflow=%s stage=%s agents=%zu autonomy=%u",
                       runtime->workflow.workflow_id,
                       umi_helix_workflow_stage_text(runtime->workflow.stage),
                       runtime->agents.count,
                       runtime->policy.autonomy_level);
    return written >= 0 && (size_t)written < output_capacity
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

UmiStatus umi_studio_helix_advance(UmiStudioAiPlatform *platform,
                                   UmiHelixWorkflowStage next_stage)
{
    UmiHelixRuntime *runtime;
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    runtime = umi_studio_ai_platform_helix(platform);
    return runtime != NULL
        ? umi_helix_workflow_advance(&runtime->workflow, next_stage)
        : UMI_STATUS_INVALID_STATE;
}
