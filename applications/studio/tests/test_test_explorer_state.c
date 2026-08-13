/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_test_explorer_state.c
 *
 * PURPOSE:
 *   Verify workspace-aware Test Explorer filters, selections and revisions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/tests.h"

static void add_test(UmiTestPlatformItemRegistry *items,
                     const char *id,
                     const char *labels)
{
    UmiTestPlatformItemSnapshot item;
    (void)memset(&item, 0, sizeof(item));
    (void)strcpy(item.id, id);
    (void)strcpy(item.name, id);
    (void)strcpy(item.suite_id, "studio.ctest");
    (void)strcpy(item.kind, "test");
    (void)strcpy(item.labels, labels);
    item.enabled = 1;
    assert(umi_test_platform_item_registry_upsert(items, &item) ==
           UMI_STATUS_OK);
}

int main(void)
{
    UmiStudioTestService *service = NULL;
    UmiStudioTestExplorerState state;
    UmiTestPlatformOperationPlan plan;
    UmiTestPlatformService *platform;
    assert(umi_studio_test_service_create(&service) == UMI_STATUS_OK);
    assert(umi_studio_test_service_set_workspace(
               service, "C:/Dev/umicom/umicom-studio", "studio", 42U) ==
           UMI_STATUS_OK);
    platform = umi_studio_test_service_platform(service);
    add_test(umi_test_platform_service_item(platform), "studio.alpha", "unit");
    add_test(umi_test_platform_service_item(platform), "studio.beta",
             "integration");
    assert(umi_studio_test_service_set_filter(service, "alpha", "unit", -1,
                                              0) == UMI_STATUS_OK);
    assert(umi_studio_test_service_plan_all(service, 3U, 1, &plan) ==
           UMI_STATUS_OK);
    assert(plan.selection.count == 1U);
    assert(plan.repeat_count == 3U);
    assert(plan.stop_on_failure == 1);
    assert(umi_studio_test_service_explorer_state(service, &state) ==
           UMI_STATUS_OK);
    assert(strcmp(state.workspace_root, "C:/Dev/umicom/umicom-studio") == 0);
    assert(state.workspace_revision == 42U);
    assert(state.selected_count == 1U);
    assert(state.revision > 1U);
    umi_studio_test_service_destroy(service);
    return 0;
}
