/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_test_explorer_hierarchy.c
 *
 * PURPOSE:
 *   Verify the bound Studio centre exposes the reusable hierarchy projection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/test_explorer_centre.h"

static void add_item(UmiTestPlatformItemRegistry *items,
                     const char *id,
                     const char *parent,
                     const char *kind)
{
    UmiTestPlatformItemSnapshot item;
    (void)memset(&item, 0, sizeof(item));
    (void)strcpy(item.id, id);
    (void)strcpy(item.parent_id, parent);
    (void)strcpy(item.name, id);
    (void)strcpy(item.kind, kind);
    item.enabled = 1;
    assert(umi_test_platform_item_registry_upsert(items, &item) ==
           UMI_STATUS_OK);
}

int main(void)
{
    UmiStudioTestService *tests = NULL;
    UmiStudioTestExplorerCentre *centre = NULL;
    UmiTestPlatformHierarchyNode nodes[2];
    UmiTestPlatformItemRegistry *items;
    size_t count = 0U;
    assert(umi_studio_test_service_create(&tests) == UMI_STATUS_OK);
    items = umi_test_platform_service_item(
        umi_studio_test_service_platform(tests));
    add_item(items, "studio.ctest", "", "suite");
    add_item(items, "studio.alpha", "studio.ctest", "test");
    assert(umi_studio_test_explorer_centre_create_bound(tests, &centre) ==
           UMI_STATUS_OK);
    assert(umi_studio_test_explorer_centre_hierarchy(centre, nodes, 2U,
                                                    &count) == UMI_STATUS_OK);
    assert(count == 2U);
    assert(nodes[0].descendant_test_count == 1U);
    assert(nodes[1].depth == 1U);
    umi_studio_test_explorer_centre_destroy(centre);
    umi_studio_test_service_destroy(tests);
    return 0;
}
