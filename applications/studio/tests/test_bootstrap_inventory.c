/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_bootstrap_inventory.c
 *
 * PURPOSE:
 *   Verify the started Studio composition exposes non-empty, introspectable
 *   command and service inventories through its public bootstrap accessors.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/platform_contract.h"

#include <assert.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiRuntimeInventorySnapshot inventory;

    (void)umi_fs_remove_tree(".umicom");
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    assert(umi_runtime_inventory_snapshot(
        umi_studio_bootstrap_command_registry(bootstrap),
        umi_studio_bootstrap_service_registry(bootstrap),
        NULL,
        &inventory) == UMI_STATUS_OK);
    assert(inventory.command_count > 0U);
    assert(inventory.service_count > 0U);
    assert(inventory.singleton_service_count > 0U);

    assert(umi_studio_bootstrap_stop(bootstrap) == UMI_STATUS_OK);
    umi_studio_bootstrap_destroy(bootstrap);
    assert(umi_fs_remove_tree(".umicom") == UMI_STATUS_OK);
    return 0;
}
