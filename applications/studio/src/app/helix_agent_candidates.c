/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/helix_agent_candidates.c
 * PURPOSE: Implement Studio candidate comparison over Framework Helix v2.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/helix_agent_candidates.h"

UmiStatus umi_studio_helix_agent_candidate_add(
    UmiStudioHelixAgentCentre *centre,
    const char *candidate_id,
    double fitness,
    const char *provenance_hash)
{
    UmiHelixOrchestrator *runtime =
        umi_studio_helix_agent_centre_runtime(centre);
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_helix_candidate_set_add(
        &runtime->candidates, candidate_id, fitness,
        fitness >= runtime->config.minimum_fitness, provenance_hash);
}

UmiStatus umi_studio_helix_agent_candidate_select(
    UmiStudioHelixAgentCentre *centre,
    UmiHelixCandidateScore *out_candidate)
{
    UmiHelixOrchestrator *runtime =
        umi_studio_helix_agent_centre_runtime(centre);
    return runtime != NULL
        ? umi_helix_candidate_set_select_best(
              &runtime->candidates, out_candidate)
        : UMI_STATUS_INVALID_ARGUMENT;
}
