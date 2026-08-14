/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_helix_agent_commands.c
 * PURPOSE: Verify plan, approval, execution and status command façades.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio/helix_agent_commands.h"
#include "umicom/studio/helix_agent_operations.h"
int main(void)
{
    UmiStudioHelixAgentCentre *centre = NULL;
    UmiStudioHelixAgentCentreConfig config =
        umi_studio_helix_agent_centre_config_default();
    UmiHelixAction action;
    UmiHelixAction compensation;
    char evidence[256];
    char status[256];
    assert(umi_studio_helix_agent_centre_create(&config, &centre) == UMI_STATUS_OK);
    assert(umi_studio_helix_agent_build_action("build/candidate", &action)
           == UMI_STATUS_OK);
    assert(umi_studio_helix_agent_plan(
        centre, "studio.op", "Build candidate", "plan-a", &action)
        == UMI_STATUS_OK);
    assert(umi_studio_helix_agent_approve(
        centre, "human", "Reviewed plan and target") == UMI_STATUS_OK);
    assert(umi_studio_helix_agent_run(centre, evidence, sizeof(evidence))
        == UMI_STATUS_OK);
    assert(strstr(evidence, "accepted") != NULL);
    assert(umi_studio_helix_agent_status(centre, status, sizeof(status))
        == UMI_STATUS_OK);
    assert(strstr(status, "succeeded") != NULL);
    assert(umi_studio_helix_agent_filesystem_action(
        "build/candidate", &compensation) == UMI_STATUS_OK);
    assert(umi_studio_helix_agent_add_compensation(centre, &compensation)
           == UMI_STATUS_OK);
    assert(umi_studio_helix_agent_begin_rollback(centre, 1) == UMI_STATUS_OK);
    assert(umi_studio_helix_agent_run_rollback(
        centre, evidence, sizeof(evidence)) == UMI_STATUS_OK);
    assert(umi_studio_helix_agent_centre_runtime(centre)->operation.state ==
           UMI_HELIX_OPERATION_ROLLED_BACK);
    umi_studio_helix_agent_centre_destroy(centre);
    return 0;
}
