/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_workbench_session_state.c
 *
 * PURPOSE:
 *   Verify the Framework workbench state codec preserves Studio chrome choices.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiUiWorkbenchState state;
    UmiUiWorkbenchState decoded;
    char encoded[UMI_UI_WORKBENCH_STATE_TEXT_CAPACITY];

    umi_ui_workbench_state_init(&state);
    state.sidebar_visible = 0;
    state.bottom_panel_size = 280;
    (void)snprintf(state.active_activity, sizeof(state.active_activity),
                   "%s", "studio.activity.framework");
    (void)snprintf(state.active_workspace_profile,
                   sizeof(state.active_workspace_profile),
                   "%s", "focus");
    assert(umi_ui_workbench_state_encode(&state, encoded,
                                         sizeof(encoded)) == UMI_STATUS_OK);
    assert(umi_ui_workbench_state_decode(encoded, &decoded) == UMI_STATUS_OK);
    assert(decoded.sidebar_visible == 0);
    assert(decoded.bottom_panel_size == 280);
    assert(strcmp(decoded.active_activity,
                  "studio.activity.framework") == 0);
    assert(strcmp(decoded.active_workspace_profile, "focus") == 0);
    return 0;
}
