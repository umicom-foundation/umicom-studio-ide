/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_helix_agent_journal.c
 * PURPOSE: Verify read-only Studio journal inspection and integrity reporting.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio/helix_agent_commands.h"
#include "umicom/studio/helix_agent_journal.h"
#include "umicom/studio/helix_agent_operations.h"
int main(void)
{
    UmiStudioHelixAgentCentre *centre = NULL;
    UmiStudioHelixAgentCentreConfig config =
        umi_studio_helix_agent_centre_config_default();
    UmiHelixAction action;
    char entry[256];
    assert(umi_studio_helix_agent_centre_create(&config, &centre) == UMI_STATUS_OK);
    assert(umi_studio_helix_agent_review_action("candidate", &action)
           == UMI_STATUS_OK);
    assert(umi_studio_helix_agent_plan(
        centre, "review-op", "Review candidate", "plan-r", &action)
        == UMI_STATUS_OK);
    assert(umi_studio_helix_agent_approve(centre, "human", "Review requested")
        == UMI_STATUS_OK);
    assert(umi_studio_helix_agent_journal_count(centre) == 2U);
    assert(umi_studio_helix_agent_journal_valid(centre));
    assert(umi_studio_helix_agent_journal_entry(centre, 0U, entry, sizeof(entry))
        == UMI_STATUS_OK);
    assert(strstr(entry, "Review candidate") != NULL);
    umi_studio_helix_agent_centre_destroy(centre);
    return 0;
}
