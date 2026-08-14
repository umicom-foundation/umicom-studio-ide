/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/helix_agent_commands.c
 * PURPOSE: Implement Studio command façades for Helix v2 governance.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/helix_agent_commands.h"

#include <stdio.h>

UmiStatus umi_studio_helix_agent_plan(UmiStudioHelixAgentCentre *centre,
                                      const char *operation_id,
                                      const char *objective,
                                      const char *plan_hash,
                                      const UmiHelixAction *action)
{
    UmiHelixOrchestrator *runtime =
        umi_studio_helix_agent_centre_runtime(centre);
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_helix_orchestrator_plan(
        runtime, operation_id, objective, plan_hash);
    if (status == UMI_STATUS_OK) status =
        umi_helix_orchestrator_prepare_action(runtime, action);
    return status;
}

static UmiStatus decide(UmiStudioHelixAgentCentre *centre,
                        UmiHelixApprovalDecision decision,
                        const char *approver,
                        const char *reason)
{
    UmiHelixOrchestrator *runtime =
        umi_studio_helix_agent_centre_runtime(centre);
    return runtime != NULL
        ? umi_helix_orchestrator_decide(
              runtime, decision, approver, reason)
        : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus umi_studio_helix_agent_approve(UmiStudioHelixAgentCentre *centre,
                                         const char *approver,
                                         const char *reason)
{
    return decide(centre, UMI_HELIX_APPROVAL_APPROVED, approver, reason);
}

UmiStatus umi_studio_helix_agent_reject(UmiStudioHelixAgentCentre *centre,
                                        const char *approver,
                                        const char *reason)
{
    return decide(centre, UMI_HELIX_APPROVAL_REJECTED, approver, reason);
}

UmiStatus umi_studio_helix_agent_run(UmiStudioHelixAgentCentre *centre,
                                     char *out_evidence,
                                     size_t capacity)
{
    UmiHelixOrchestrator *runtime =
        umi_studio_helix_agent_centre_runtime(centre);
    return runtime != NULL
        ? umi_helix_orchestrator_execute(runtime, out_evidence, capacity)
        : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus umi_studio_helix_agent_add_compensation(
    UmiStudioHelixAgentCentre *centre,
    const UmiHelixAction *action)
{
    UmiHelixOrchestrator *runtime =
        umi_studio_helix_agent_centre_runtime(centre);
    return runtime != NULL
        ? umi_helix_compensation_plan_add(&runtime->rollback, action)
        : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus umi_studio_helix_agent_begin_rollback(
    UmiStudioHelixAgentCentre *centre,
    int human_approved)
{
    UmiHelixOrchestrator *runtime =
        umi_studio_helix_agent_centre_runtime(centre);
    return runtime != NULL
        ? umi_helix_orchestrator_begin_rollback(runtime, human_approved)
        : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus umi_studio_helix_agent_run_rollback(
    UmiStudioHelixAgentCentre *centre,
    char *out_evidence,
    size_t capacity)
{
    UmiHelixOrchestrator *runtime =
        umi_studio_helix_agent_centre_runtime(centre);
    return runtime != NULL
        ? umi_helix_orchestrator_execute_rollback(
              runtime, out_evidence, capacity)
        : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus umi_studio_helix_agent_status(UmiStudioHelixAgentCentre *centre,
                                        char *out_text,
                                        size_t capacity)
{
    UmiHelixOrchestrator *runtime =
        umi_studio_helix_agent_centre_runtime(centre);
    int written;
    if (runtime == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(out_text, capacity,
        "Helix Agent Runtime v2: state=%s agents=%zu approvals=%zu journal=%zu retries=%u",
        runtime->operation_ready
            ? umi_helix_operation_state_text(runtime->operation.state) : "idle",
        runtime->agents.count, runtime->approvals.count, runtime->journal.count,
        runtime->operation.retry_count);
    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}
