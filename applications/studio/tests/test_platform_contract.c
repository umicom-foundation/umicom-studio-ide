/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_platform_contract.c
 *
 * PURPOSE:
 *   Verify the complete Studio-owned command/service composition contract after
 *   the Framework composition root has started.
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
    UmiStudioPlatformContractSnapshot snapshot;

    (void)umi_fs_remove_tree(".umicom");
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_platform_contract_capture_bootstrap(
        bootstrap, &snapshot) == UMI_STATUS_OK);
    assert(umi_studio_platform_contract_validate(&snapshot) == UMI_STATUS_OK);
    assert(snapshot.valid);
    assert(snapshot.core_commands_complete);
    assert(snapshot.workbench_commands_complete);
    assert(snapshot.contributed_commands_complete);
    assert(snapshot.required_services_complete);
    assert(snapshot.runtime.command_count >= snapshot.expected_minimum_command_count);
    assert(snapshot.runtime.service_count >= snapshot.required_service_count);

    assert(umi_studio_bootstrap_stop(bootstrap) == UMI_STATUS_OK);
    umi_studio_bootstrap_destroy(bootstrap);
    assert(umi_fs_remove_tree(".umicom") == UMI_STATUS_OK);
    return 0;
}
