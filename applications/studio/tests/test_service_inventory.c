/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_service_inventory.c
 *
 * PURPOSE:
 *   Verify the stable minimum Studio service identities published into the
 *   Framework service registry by the composition root.
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
    UmiServiceRegistry *registry;
    size_t index;

    (void)umi_fs_remove_tree(".umicom");
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    registry = umi_studio_bootstrap_service_registry(bootstrap);
    assert(registry != NULL);

    for (index = 0U;
         index < umi_studio_platform_contract_required_service_count();
         ++index) {
        const char *id = umi_studio_platform_contract_required_service_id(index);
        assert(id != NULL);
        assert(umi_runtime_inventory_has_service(registry, id));
    }

    assert(umi_studio_bootstrap_stop(bootstrap) == UMI_STATUS_OK);
    umi_studio_bootstrap_destroy(bootstrap);
    assert(umi_fs_remove_tree(".umicom") == UMI_STATUS_OK);
    return 0;
}
