/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_helix_agent_views.c
 * PURPOSE: Verify Studio publishes Helix v2 operational pane models.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio/helix_agent_views.h"
int main(void)
{
    UmiStudioHelixAgentCentre *centre = NULL;
    UmiStudioHelixAgentCentreConfig config =
        umi_studio_helix_agent_centre_config_default();
    UmiUiViewModel *view = NULL;
    UmiUiValue value;
    assert(strcmp(umi_studio_helix_agent_capability_id(),
                  "umicom.studio.helix-agent-runtime") == 0);
    assert(umi_studio_helix_agent_centre_create(&config, &centre) == UMI_STATUS_OK);
    assert(umi_studio_helix_agent_view_create(
        centre, UMI_STUDIO_HELIX_PANE_OVERVIEW, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(view, "helix.agents", &value)
           == UMI_STATUS_OK && value.integer_value == 5);
    umi_ui_view_model_destroy(view);
    umi_studio_helix_agent_centre_destroy(centre);
    return 0;
}
