/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_registry_extension_compatibility.c
 *
 * PURPOSE:
 *   Prove the Studio contract remains valid when an extension contributes an
 *   additional command, preventing future optional features from reintroducing
 *   the fragile exact-registry-count regression.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/platform_contract.h"

#include <assert.h>
#include <string.h>

static UmiStatus extension_handler(void *user_data,
                                   const char *argument,
                                   char *out_message,
                                   size_t message_capacity)
{
    (void)user_data;
    (void)argument;
    if (out_message != NULL && message_capacity > 0U) {
        out_message[0] = '\0';
    }
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiCommandRegistry *registry;
    UmiCommandDescriptor descriptor;
    UmiStudioPlatformContractSnapshot before;
    UmiStudioPlatformContractSnapshot after;

    (void)umi_fs_remove_tree(".umicom");
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_platform_contract_capture_bootstrap(
        bootstrap, &before) == UMI_STATUS_OK);

    registry = umi_studio_bootstrap_command_registry(bootstrap);
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.command_id = "extension.example.command";
    descriptor.title = "Extension command";
    descriptor.category = "Extensions";
    descriptor.description = "Regression test extension";
    descriptor.required_permission = NULL;
    descriptor.handler = extension_handler;
    assert(umi_command_registry_register(registry, &descriptor) == UMI_STATUS_OK);

    assert(umi_studio_platform_contract_capture_bootstrap(
        bootstrap, &after) == UMI_STATUS_OK);
    assert(after.runtime.command_count == before.runtime.command_count + 1U);
    assert(umi_studio_platform_contract_validate(&after) == UMI_STATUS_OK);

    assert(umi_studio_bootstrap_stop(bootstrap) == UMI_STATUS_OK);
    umi_studio_bootstrap_destroy(bootstrap);
    assert(umi_fs_remove_tree(".umicom") == UMI_STATUS_OK);
    return 0;
}
