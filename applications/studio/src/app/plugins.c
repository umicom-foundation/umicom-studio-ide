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

#include <string.h>

#include "umicom/studio/operations.h"

static UmiStatus register_discovered(const char *manifest_path,
                                     const UmiPluginManifest *manifest,
                                     void *user_data)
{
    UmiPluginHost *host = (UmiPluginHost *)user_data;
    UmiPluginPermissionDecision decision;
    return umi_plugin_host_register_manifest(host,
                                             manifest,
                                             manifest_path,
                                             &decision);
}

UmiStatus umi_studio_plugins_report(UmiStudioServices *services,
                                    UmiStudioPluginReport *out_report)
{
    UmiStudioOperations *operations;
    UmiPluginHost *host;
    UmiPluginRegistry *registry;
    size_t index;

    if (services == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    operations = umi_studio_services_operations(services);
    host = umi_studio_operations_plugins(operations);
    if (host == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }
    registry = umi_plugin_host_registry(host);
    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->registered = umi_plugin_registry_count(registry);
    out_report->contributions = umi_plugin_contribution_registry_count(
        umi_plugin_host_contributions(host));

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
    UmiPluginHost *host;
    UmiStatus status;
    uint64_t now;

    if (services == NULL || root == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    operations = umi_studio_services_operations(services);
    host = umi_studio_operations_plugins(operations);
    if (host == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_plugin_discover(root,
                                 recursive,
                                 register_discovered,
                                 host,
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
