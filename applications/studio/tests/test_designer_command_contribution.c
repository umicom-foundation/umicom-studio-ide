/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_designer_command_contribution.c
 *
 * PURPOSE:
 *   Verify the two reusable Framework Designer commands that Studio publishes
 *   alongside its own core and workbench command sets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/platform_contract.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiCommandRegistry *registry;

    (void)umi_fs_remove_tree(".umicom");
    assert(umi_studio_platform_contract_contributed_command_count() == 2U);
    assert(strcmp(umi_studio_platform_contract_contributed_command_id(0U),
                  "designer.undo") == 0);
    assert(strcmp(umi_studio_platform_contract_contributed_command_id(1U),
                  "designer.redo") == 0);

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    registry = umi_studio_bootstrap_command_registry(bootstrap);
    assert(umi_runtime_inventory_has_command(registry, "designer.undo"));
    assert(umi_runtime_inventory_has_command(registry, "designer.redo"));

    assert(umi_studio_bootstrap_stop(bootstrap) == UMI_STATUS_OK);
    umi_studio_bootstrap_destroy(bootstrap);
    assert(umi_fs_remove_tree(".umicom") == UMI_STATUS_OK);
    return 0;
}
