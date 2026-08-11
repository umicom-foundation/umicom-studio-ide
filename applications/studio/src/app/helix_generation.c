/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/helix_generation.c
 *
 * PURPOSE:
 *   Apply Framework Helix release gates to Studio candidates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The call combines test evidence, fitness, human approval and rollback readiness before promotion can succeed.
 */

#include "umicom/studio/helix_generation.h"

UmiStatus umi_studio_helix_can_promote(UmiStudioAiPlatform *platform,
                                       const UmiHelixFitness *fitness,
                                       uint32_t required_checks,
                                       uint32_t passed_checks,
                                       int human_approved,
                                       int rollback_ready)
{
    UmiHelixRuntime *runtime = umi_studio_ai_platform_helix(platform);
    UmiHelixReleaseGate gate;
    if (runtime == NULL || fitness == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    gate.required_checks = required_checks;
    gate.passed_checks = passed_checks;
    gate.human_approved = human_approved;
    gate.rollback_ready = rollback_ready;
    return umi_helix_release_gate_check(&gate, fitness, &runtime->policy);
}
