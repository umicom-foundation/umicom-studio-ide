/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_authorengine_views.c
 *
 * PURPOSE:
 *   Verify Studio's AI panes render the shared Framework integration service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/ai_platform.h"
#include "umicom/ui/command_view.h"

int main(void)
{
    UmiStudioAiPlatform *platform = NULL;
    UmiAiAuthorEngineService *service;
    UmiUiViewModel *view = NULL;
    UmiUiValue value;
    UmiUiCommandViewAction action;
    assert(umi_studio_ai_platform_create(&platform) == UMI_STATUS_OK);
    service = umi_studio_ai_platform_authorengine(platform);
    assert(umi_ai_ui_authorengine_overview_view_create(
        "studio.pane.ai", service, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
        view, "authorengine.runtimes", &value) == UMI_STATUS_OK);
    assert(value.integer_value == 3);
    assert(umi_ui_command_view_action_at(view, 0U, &action) == UMI_STATUS_OK);
    assert(strcmp(action.action_id, "studio.action.ai.refresh-health") == 0);
    umi_ui_view_model_destroy(view);
    assert(umi_ai_ui_runtime_catalogue_view_create(
        "studio.pane.ai-runtimes", service, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
        view, "ai-runtimes.row-count", &value) == UMI_STATUS_OK);
    assert(value.integer_value == 3);
    umi_ui_view_model_destroy(view);
    assert(umi_ai_ui_context_view_create(
        "studio.pane.ai-context", service, &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    assert(umi_ai_ui_sessions_view_create(
        "studio.pane.ai-sessions", service, &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    assert(umi_ai_ui_privacy_view_create(
        "studio.pane.ai-privacy", service, &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    umi_studio_ai_platform_destroy(platform);
    return 0;
}
