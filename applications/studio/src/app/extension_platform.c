/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/extension_platform.c
 *
 * PURPOSE:
 *   Initialise Studio-owned extension points and catalogue records while
 *   delegating all generic management behaviour to Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/studio/extension_platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiStudioExtensionPlatform {
    UmiPluginManager *manager;
    UmiPluginPolicy policy;
};

static UmiStatus add_extension_point(UmiPluginManager *manager,
                                     const char *extension_id,
                                     const char *schema,
                                     UmiPluginExtensionCardinality cardinality,
                                     int dynamic)
{
    UmiPluginExtensionPoint point;
    (void)memset(&point, 0, sizeof(point));
    (void)snprintf(point.extension_id, sizeof(point.extension_id),
                   "%s", extension_id);
    (void)snprintf(point.owner_id, sizeof(point.owner_id),
                   "%s", "org.umicom.studio");
    (void)snprintf(point.schema, sizeof(point.schema), "%s", schema);
    point.cardinality = cardinality;
    point.dynamic = dynamic;
    return umi_plugin_extension_point_registry_add(
        umi_plugin_manager_extension_points(manager), &point);
}

static UmiStatus register_extension_points(UmiPluginManager *manager)
{
    UmiStatus status;
    status = add_extension_point(manager, "studio.commands", "command-id",
                                 UMI_PLUGIN_EXTENSION_MANY, 1);
    if (status == UMI_STATUS_OK) status = add_extension_point(
        manager, "studio.panes", "pane-id", UMI_PLUGIN_EXTENSION_MANY, 1);
    if (status == UMI_STATUS_OK) status = add_extension_point(
        manager, "studio.menus", "menu-location", UMI_PLUGIN_EXTENSION_MANY, 1);
    if (status == UMI_STATUS_OK) status = add_extension_point(
        manager, "studio.tools", "tool-id", UMI_PLUGIN_EXTENSION_MANY, 1);
    if (status == UMI_STATUS_OK) status = add_extension_point(
        manager, "studio.providers", "provider-id", UMI_PLUGIN_EXTENSION_MANY, 1);
    if (status == UMI_STATUS_OK) status = add_extension_point(
        manager, "studio.languages", "language-id", UMI_PLUGIN_EXTENSION_MANY, 1);
    if (status == UMI_STATUS_OK) status = add_extension_point(
        manager, "studio.debuggers", "debug-adapter-id", UMI_PLUGIN_EXTENSION_MANY, 0);
    if (status == UMI_STATUS_OK) status = add_extension_point(
        manager, "studio.ai-tools", "ai-tool-id", UMI_PLUGIN_EXTENSION_MANY, 0);
    return status;
}

static UmiStatus add_catalogue_entry(UmiPluginManager *manager,
                                     const char *plugin_id,
                                     const char *name,
                                     const char *description,
                                     UmiVersion version)
{
    UmiPluginCatalogueEntry entry;
    (void)memset(&entry, 0, sizeof(entry));
    (void)snprintf(entry.plugin_id, sizeof(entry.plugin_id), "%s", plugin_id);
    (void)snprintf(entry.display_name, sizeof(entry.display_name), "%s", name);
    (void)snprintf(entry.publisher, sizeof(entry.publisher),
                   "%s", "Umicom Foundation");
    (void)snprintf(entry.description, sizeof(entry.description),
                   "%s", description);
    entry.version = version;
    entry.required_abi = UMICOM_FRAMEWORK_ABI_VERSION;
    entry.verified = 1;
    entry.rating_milli = 5000U;
    return umi_plugin_catalogue_add(umi_plugin_manager_catalogue(manager), &entry);
}

static UmiStatus register_builtin_catalogue(UmiPluginManager *manager)
{
    UmiVersion version = {1U, 0U, 0U};
    UmiStatus status = add_catalogue_entry(
        manager, "org.umicom.c-tools", "Umicom C23 Tools",
        "C23 language intelligence, build profiles and native diagnostics.", version);
    if (status == UMI_STATUS_OK) status = add_catalogue_entry(
        manager, "org.umicom.assembly-tools", "Umicom Assembly Tools",
        "x86-64, AArch64 and RISC-V assembly navigation and inspection.", version);
    if (status == UMI_STATUS_OK) status = add_catalogue_entry(
        manager, "org.umicom.authorengine", "AuthorEngine Integration",
        "Provider-neutral AI writing and coding integration for Studio.", version);
    return status;
}

UmiStatus umi_studio_extension_platform_create(
    UmiPluginHost *host,
    UmiStudioExtensionPlatform **out_platform)
{
    UmiStudioExtensionPlatform *platform;
    UmiStatus status;
    if (host == NULL || out_platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_platform = NULL;
    platform = (UmiStudioExtensionPlatform *)calloc(1U, sizeof(*platform));
    if (platform == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    platform->policy = umi_plugin_policy_default();
    platform->policy.minimum_trust = UMI_PLUGIN_TRUST_LOCAL;
    platform->policy.untrusted_isolation = UMI_PLUGIN_ISOLATION_RESTRICTED_PROCESS;
    status = umi_plugin_manager_create(host, &platform->policy, &platform->manager);
    if (status == UMI_STATUS_OK) status = register_extension_points(platform->manager);
    if (status == UMI_STATUS_OK) status = register_builtin_catalogue(platform->manager);
    if (status != UMI_STATUS_OK) {
        umi_studio_extension_platform_destroy(platform);
        return status;
    }
    *out_platform = platform;
    return UMI_STATUS_OK;
}

void umi_studio_extension_platform_destroy(UmiStudioExtensionPlatform *platform)
{
    if (platform == NULL) return;
    umi_plugin_manager_destroy(platform->manager);
    free(platform);
}

UmiPluginManager *umi_studio_extension_platform_manager(
    UmiStudioExtensionPlatform *platform)
{
    return platform != NULL ? platform->manager : NULL;
}

UmiStatus umi_studio_extension_platform_snapshot(
    const UmiStudioExtensionPlatform *platform,
    UmiStudioExtensionPlatformSnapshot *out_snapshot)
{
    UmiPluginManagerSnapshot manager_snapshot;
    UmiStatus status;
    if (platform == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    status = umi_plugin_manager_snapshot(platform->manager, &manager_snapshot);
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->revision = manager_snapshot.revision;
    out_snapshot->installed = manager_snapshot.installed;
    out_snapshot->enabled = manager_snapshot.enabled;
    out_snapshot->active = manager_snapshot.active;
    out_snapshot->failed = manager_snapshot.failed;
    out_snapshot->contributions = manager_snapshot.contributions;
    out_snapshot->extension_points = manager_snapshot.extension_points;
    out_snapshot->catalogue_entries = manager_snapshot.catalogue_entries;
    out_snapshot->pending_updates = manager_snapshot.pending_updates;
    out_snapshot->audit_events = manager_snapshot.audit_events;
    out_snapshot->permission_grants = umi_permission_set_count(
        umi_plugin_host_grants(umi_plugin_manager_host(platform->manager)));
    out_snapshot->native_extensions_allowed = platform->policy.allow_native_code;
    out_snapshot->default_untrusted_isolation = platform->policy.untrusted_isolation;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_extension_platform_set_enabled(
    UmiStudioExtensionPlatform *platform,
    const char *plugin_id,
    int enabled,
    uint64_t timestamp_ms)
{
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_plugin_manager_set_enabled(
        platform->manager, plugin_id, enabled, timestamp_ms);
}
