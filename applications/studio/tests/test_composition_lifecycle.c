/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_composition_lifecycle.c
 *
 * PURPOSE:
 *   Verify the Studio composition can complete two independent create/start/
 *   contract-validation/stop/destroy cycles without leaking registry state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/platform_contract.h"

#include <assert.h>

static void run_cycle(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioPlatformContractSnapshot snapshot;

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_platform_contract_capture_bootstrap(
        bootstrap, &snapshot) == UMI_STATUS_OK);
    assert(umi_studio_platform_contract_validate(&snapshot) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_stop(bootstrap) == UMI_STATUS_OK);
    umi_studio_bootstrap_destroy(bootstrap);
}

int main(void)
{
    (void)umi_fs_remove_tree(".umicom");
    run_cycle();
    run_cycle();
    assert(umi_fs_remove_tree(".umicom") == UMI_STATUS_OK);
    return 0;
}
