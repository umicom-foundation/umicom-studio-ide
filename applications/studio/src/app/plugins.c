/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/plugins.c
 *
 * PURPOSE:
 *   Discover inspectable manifests, enforce Framework permission grants and
 *   report Studio plug-in catalogue state without loading unapproved code.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/plugins.h"

#include <stdio.h>
#include <string.h>

#include "umicom/studio/operations.h"

typedef struct UmiStudioPluginDiscoveryContext {
    UmiPluginManager *manager;
    uint64_t timestamp_ms;
} UmiStudioPluginDiscoveryContext;

static UmiStatus register_discovered(const char *manifest_path,
                                     const UmiPluginManifest *manifest,
                                     void *user_data)
{
    UmiStudioPluginDiscoveryContext *context =
        (UmiStudioPluginDiscoveryContext *)user_data;
    UmiPluginPolicyDecision decision;
    if (context == NULL || context->manager == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_plugin_manager_register(context->manager,
                                       manifest,
                                       manifest_path,
                                       UMI_PLUGIN_TRUST_LOCAL,
                                       context->timestamp_ms,
                                       &decision);
}

UmiStatus umi_studio_plugins_report(UmiStudioServices *services,
                                    UmiStudioPluginReport *out_report)
{
    UmiPluginHost *host;
    UmiStudioExtensionPlatform *platform;
    UmiPluginRegistry *registry;
    size_t index;
    UmiStudioExtensionPlatformSnapshot platform_snapshot;

    if (services == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    platform = umi_studio_services_extension_platform(services);
    host = platform != NULL
        ? umi_plugin_manager_host(umi_studio_extension_platform_manager(platform))
        : NULL;
    if (host == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }
    registry = umi_plugin_host_registry(host);
    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->registered = umi_plugin_registry_count(registry);
    out_report->contributions = umi_plugin_contribution_registry_count(
        umi_plugin_host_contributions(host));
    if (umi_studio_extension_platform_snapshot(
            platform, &platform_snapshot) == UMI_STATUS_OK) {
        out_report->extension_points = platform_snapshot.extension_points;
        out_report->catalogue_entries = platform_snapshot.catalogue_entries;
        out_report->audit_events = platform_snapshot.audit_events;
        out_report->permission_grants = platform_snapshot.permission_grants;
        (void)snprintf(out_report->default_isolation,
                       sizeof(out_report->default_isolation), "%s",
                       umi_plugin_isolation_mode_text(
                           platform_snapshot.default_untrusted_isolation));
    }

    for (index = 0U; index < out_report->registered; ++index) {
        UmiPluginRecord record;
        if (umi_plugin_registry_at(registry, index, &record) != UMI_STATUS_OK) {
            continue;
        }
        if (record.enabled) {
            ++out_report->enabled;
        }
        if (record.state == UMI_PLUGIN_FAILED) {
            ++out_report->failed;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_plugins_discover(UmiStudioServices *services,
                                      const char *root,
                                      int recursive,
                                      UmiStudioPluginReport *out_report)
{
    UmiStudioOperations *operations;
    UmiPluginDiscoveryReport discovery;
    UmiStudioExtensionPlatform *platform;
    UmiStudioPluginDiscoveryContext context;
    UmiStatus status;
    uint64_t now;

    if (services == NULL || root == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    operations = umi_studio_services_operations(services);
    platform = umi_studio_services_extension_platform(services);
    if (platform == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    context.manager = umi_studio_extension_platform_manager(platform);
    context.timestamp_ms = umi_studio_operations_now(operations);

    status = umi_plugin_discover(root,
                                 recursive,
                                 register_discovered,
                                 &context,
                                 &discovery);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_studio_plugins_report(services, out_report);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    out_report->files_examined = discovery.files_examined;
    out_report->manifests_loaded = discovery.manifests_loaded;
    out_report->invalid_manifests = discovery.invalid_manifests;
    now = umi_studio_operations_now(operations);
    return umi_metrics_gauge_set(umi_studio_operations_metrics(operations),
                                 "studio.plugins.discovered",
                                 (double)out_report->registered,
                                 now);
}
