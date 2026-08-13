/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_extension_platform.c
 *
 * PURPOSE:
 *   Verify Studio composes Framework Extension SDK v2 services without owning
 *   a competing extension manager implementation.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/extension_platform.h"

int main(void)
{
    UmiPluginHost *host = NULL;
    UmiStudioExtensionPlatform *platform = NULL;
    UmiStudioExtensionPlatformSnapshot snapshot;
    UmiPluginManifest manifest;
    UmiPluginPolicyDecision decision;
    assert(umi_plugin_host_create(&host) == UMI_STATUS_OK);
    assert(umi_permission_set_add(umi_plugin_host_grants(host), "*") == UMI_STATUS_OK);
    assert(umi_studio_extension_platform_create(host, &platform) == UMI_STATUS_OK);
    assert(umi_studio_extension_platform_snapshot(platform, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.extension_points == 8U);
    assert(snapshot.catalogue_entries == 3U);
    assert(snapshot.permission_grants == 1U);
    assert(snapshot.default_untrusted_isolation ==
           UMI_PLUGIN_ISOLATION_RESTRICTED_PROCESS);
    (void)memset(&manifest, 0, sizeof(manifest));
    (void)strcpy(manifest.plugin_id, "org.example.studio");
    (void)strcpy(manifest.display_name, "Studio Example");
    (void)strcpy(manifest.library_path, "example.dll");
    manifest.version = (UmiVersion){1U, 0U, 0U};
    manifest.required_abi = UMICOM_FRAMEWORK_ABI_VERSION;
    assert(umi_plugin_manager_register(
        umi_studio_extension_platform_manager(platform),
        &manifest, "example.umicom-plugin", UMI_PLUGIN_TRUST_LOCAL,
        100U, &decision) == UMI_STATUS_OK);
    assert(umi_studio_extension_platform_set_enabled(
        platform, manifest.plugin_id, 0, 200U) == UMI_STATUS_OK);
    assert(umi_studio_extension_platform_snapshot(platform, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.installed == 1U);
    assert(snapshot.enabled == 0U);
    assert(snapshot.audit_events == 2U);
    umi_studio_extension_platform_destroy(platform);
    umi_plugin_host_destroy(host);
    return 0;
}
