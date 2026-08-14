/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_helix_agent_candidates.c
 * PURPOSE: Verify Studio applies its fitness threshold before selection.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio/helix_agent_candidates.h"
int main(void)
{
    UmiStudioHelixAgentCentre *centre = NULL;
    UmiStudioHelixAgentCentreConfig config =
        umi_studio_helix_agent_centre_config_default();
    UmiHelixCandidateScore selected;
    assert(umi_studio_helix_agent_centre_create(&config, &centre) == UMI_STATUS_OK);
    assert(umi_studio_helix_agent_candidate_add(centre, "low", 0.79, "p1")
           == UMI_STATUS_OK);
    assert(umi_studio_helix_agent_candidate_add(centre, "fit", 0.88, "p2")
           == UMI_STATUS_OK);
    assert(umi_studio_helix_agent_candidate_select(centre, &selected)
           == UMI_STATUS_OK);
    assert(strcmp(selected.candidate_id, "fit") == 0);
    umi_studio_helix_agent_centre_destroy(centre);
    return 0;
}
