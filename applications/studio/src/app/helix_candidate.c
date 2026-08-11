/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/helix_candidate.c
 *
 * PURPOSE:
 *   Create bounded Studio Helix candidate records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The helper creates only metadata; source mutation remains a later controlled operation executed by authorised agents.
 */

#include "umicom/studio/helix_candidate.h"

#include <stdio.h>

UmiStatus umi_studio_helix_candidate(const char *candidate_id,
                                     const char *plan_id,
                                     const char *workspace,
                                     UmiHelixCandidate *candidate)
{
    int a;
    int b;
    int c;
    if (candidate_id == NULL || plan_id == NULL || workspace == NULL ||
        candidate == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_helix_candidate_init(candidate);
    a = snprintf(candidate->candidate_id, sizeof(candidate->candidate_id),
                 "%s", candidate_id);
    b = snprintf(candidate->plan_id, sizeof(candidate->plan_id), "%s", plan_id);
    c = snprintf(candidate->workspace, sizeof(candidate->workspace), "%s", workspace);
    if (a < 0 || b < 0 || c < 0 ||
        (size_t)a >= sizeof(candidate->candidate_id) ||
        (size_t)b >= sizeof(candidate->plan_id) ||
        (size_t)c >= sizeof(candidate->workspace)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    candidate->state = UMI_HELIX_CANDIDATE_DRAFT;
    return UMI_STATUS_OK;
}
