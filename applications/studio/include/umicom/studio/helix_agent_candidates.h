/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/helix_agent_candidates.h
 * PURPOSE: Expose candidate scoring and comparison through Studio composition.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Studio does not invent a separate ranking algorithm; it uses
 * Framework's deterministic eligible-candidate selection contract. */
#ifndef UMICOM_STUDIO_HELIX_AGENT_CANDIDATES_H
#define UMICOM_STUDIO_HELIX_AGENT_CANDIDATES_H

#include "umicom/studio/helix_agent_centre.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_helix_agent_candidate_add(
    UmiStudioHelixAgentCentre *centre,
    const char *candidate_id,
    double fitness,
    const char *provenance_hash);
UmiStatus umi_studio_helix_agent_candidate_select(
    UmiStudioHelixAgentCentre *centre,
    UmiHelixCandidateScore *out_candidate);

#ifdef __cplusplus
}
#endif

#endif
