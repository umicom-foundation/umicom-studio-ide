/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_ai_coding_views.c
 *
 * PURPOSE:
 *   Verify Studio's coding-assistant panes project live Framework service state
 *   without introducing toolkit-specific business logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/ai_platform.h"

int main(void)
{
    UmiStudioAiPlatform *platform = NULL;
    UmiAiCodingAssistantService *assistant;
    UmiUiViewModel *view = NULL;
    UmiUiValue value;

    assert(umi_studio_ai_platform_create(&platform) == UMI_STATUS_OK);
    assistant = umi_studio_ai_platform_coding_assistant(platform);
    assert(umi_ai_ui_coding_assistant_view_create(
        "studio.pane.ai-coding", assistant, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
        view, "ai-coding.repository-files", &value) == UMI_STATUS_OK);
    assert(value.integer_value == 4);
    umi_ui_view_model_destroy(view);
    view = NULL;
    assert(umi_ai_ui_coding_repository_view_create(
        "studio.pane.ai-coding-context", assistant, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
        view, "ai-coding-context.row-count", &value) == UMI_STATUS_OK);
    assert(value.integer_value == 4);
    umi_ui_view_model_destroy(view);
    view = NULL;
    assert(umi_ai_ui_coding_patch_view_create(
        "studio.pane.ai-patch-review", assistant, "", &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
        view, "ai-coding-patch.state", &value) == UMI_STATUS_OK);
    assert(strcmp(value.string_value, "no patch") == 0);
    umi_ui_view_model_destroy(view);
    umi_studio_ai_platform_destroy(platform);
    return 0;
}
