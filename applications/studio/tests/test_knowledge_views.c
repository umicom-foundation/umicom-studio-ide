/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_knowledge_views.c
 * PURPOSE: Verify Studio panes delegate to Framework Knowledge Centre views.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio/knowledge_views.h"

int main(void)
{
    UmiStudioAiPlatformConfig config = umi_studio_ai_platform_config_default();
    UmiStudioAiPlatform *platform = NULL;
    UmiUiViewModel *view = NULL;
    UmiUiValue value;
    config.knowledge_source_capacity = 8U;
    config.knowledge_vector_capacity = 32U;
    assert(umi_studio_ai_platform_create_configured(&config, &platform)
           == UMI_STATUS_OK);
    assert(umi_studio_knowledge_overview_view(
        platform, "studio.test.knowledge", &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
        view, "knowledge.collections", &value) == UMI_STATUS_OK);
    assert(value.integer_value == 3);
    umi_ui_view_model_destroy(view);
    assert(umi_studio_knowledge_collections_view(
        platform, "studio.test.collections", &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    assert(umi_studio_knowledge_sources_view(
        platform, "studio.test.sources", &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    assert(umi_studio_knowledge_search_view(
        platform, "studio.test.search", "nothing indexed", &view)
        == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    umi_studio_ai_platform_destroy(platform);
    return 0;
}
