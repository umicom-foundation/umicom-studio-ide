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
    state.editor_split_mode = UMI_UI_EDITOR_SPLIT_COLUMNS;
    state.editor_split_ratio = 4200;
    (void)snprintf(state.active_editor_group,
                   sizeof(state.active_editor_group), "%s",
                   UMI_UI_SECONDARY_EDITOR_GROUP_ID);
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
    assert(decoded.editor_split_mode == UMI_UI_EDITOR_SPLIT_COLUMNS);
    assert(decoded.editor_split_ratio == 4200);
    assert(strcmp(decoded.active_editor_group,
                  UMI_UI_SECONDARY_EDITOR_GROUP_ID) == 0);
    assert(umi_ui_workbench_state_decode(
               "v2|explorer|files|develop|welcome|focus|1|0|1|280|360|240|7",
               &decoded) == UMI_STATUS_OK);
    assert(decoded.editor_split_mode == UMI_UI_EDITOR_SPLIT_SINGLE);
    assert(decoded.editor_split_ratio == UMI_UI_EDITOR_SPLIT_RATIO_DEFAULT);
    return 0;
}
