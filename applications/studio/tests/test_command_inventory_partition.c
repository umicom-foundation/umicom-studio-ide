/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_command_inventory_partition.c
 *
 * PURPOSE:
 *   Verify declared core/workbench command counts and show that the extensible
 *   runtime registry may legitimately contain additional designer commands.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/commands.h"
#include "umicom/studio/platform_contract.h"
#include "umicom/studio/workbench_commands.h"

#include <assert.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioPlatformContractSnapshot snapshot;

    (void)umi_fs_remove_tree(".umicom");
    assert(umi_studio_platform_contract_core_command_count() ==
           UMI_STUDIO_CORE_COMMAND_COUNT);
    assert(umi_studio_platform_contract_workbench_command_count() ==
           UMI_STUDIO_WORKBENCH_COMMAND_COUNT);

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_platform_contract_capture_bootstrap(
        bootstrap, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.expected_studio_command_count ==
           UMI_STUDIO_CORE_COMMAND_COUNT + UMI_STUDIO_WORKBENCH_COMMAND_COUNT);
    assert(snapshot.runtime.command_count >= snapshot.expected_minimum_command_count);

    assert(umi_studio_bootstrap_stop(bootstrap) == UMI_STATUS_OK);
    umi_studio_bootstrap_destroy(bootstrap);
    assert(umi_fs_remove_tree(".umicom") == UMI_STATUS_OK);
    return 0;
}
