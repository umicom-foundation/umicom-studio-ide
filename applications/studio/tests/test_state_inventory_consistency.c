/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_state_inventory_consistency.c
 *
 * PURPOSE:
 *   Verify the operational state report reflects the authoritative runtime
 *   registry counts instead of a duplicated compile-time command total.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/platform_contract.h"
#include "umicom/studio/state.h"

#include <assert.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioStateReport state;
    UmiStudioPlatformContractSnapshot contract;

    (void)umi_fs_remove_tree(".umicom");
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_state_capture(bootstrap, &state) == UMI_STATUS_OK);
    assert(umi_studio_platform_contract_capture_bootstrap(
        bootstrap, &contract) == UMI_STATUS_OK);
    assert(state.command_count == contract.runtime.command_count);
    assert(state.service_count == contract.runtime.service_count);
    assert(state.command_count >= contract.expected_minimum_command_count);

    assert(umi_studio_bootstrap_stop(bootstrap) == UMI_STATUS_OK);
    umi_studio_bootstrap_destroy(bootstrap);
    assert(umi_fs_remove_tree(".umicom") == UMI_STATUS_OK);
    return 0;
}
