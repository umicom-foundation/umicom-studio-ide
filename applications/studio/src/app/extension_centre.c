/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/extension_centre.c
 *
 * PURPOSE:
 *   Implement Studio Extension Centre composition over Framework Extension SDK
 *   manager, catalogue, permissions, audit and toolkit-neutral views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio composes reusable Framework capabilities here. Generic models and
 * engines stay in Framework so the same mechanisms can serve future products.
 */
#include "umicom/studio/extension_centre.h"
#include <string.h>

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

static UmiPluginManager *extension_manager(UmiStudioServices *services)
{
    UmiStudioExtensionPlatform *platform =
        umi_studio_services_extension_platform(services);
    return platform != NULL
        ? umi_studio_extension_platform_manager(platform) : NULL;
}

UmiStatus umi_studio_extension_centre_snapshot(
    UmiStudioServices *services,
    UmiStudioExtensionCentreSnapshot *out)
{
    UmiStudioExtensionPlatform *platform;
    UmiStudioExtensionPlatformSnapshot platform_snapshot;
    UmiStatus status;
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out, 0, sizeof(*out));
    out->struct_size = (uint32_t)sizeof(*out);
    out->api_version = 2U;
    copy_text(out->area_id, sizeof(out->area_id), "studio.extension-centre");
    copy_text(out->view_type, sizeof(out->view_type), "studio.extensions-installed");
    copy_text(out->title, sizeof(out->title), "Extension Centre");
    copy_text(out->summary, sizeof(out->summary),
              "Installed extensions, catalogue, dependencies, permissions, trust and audit evidence.");
    out->revision = 1U;
    out->available = 1;
    if (services == NULL) return UMI_STATUS_OK;
    platform = umi_studio_services_extension_platform(services);
    if (platform == NULL) {
        out->available = 0;
        return UMI_STATUS_UNAVAILABLE;
    }
    status = umi_studio_extension_platform_snapshot(platform, &platform_snapshot);
    if (status != UMI_STATUS_OK) return status;
    out->revision = platform_snapshot.revision;
    out->installed = platform_snapshot.installed;
    out->enabled = platform_snapshot.enabled;
    out->active = platform_snapshot.active;
    out->failed = platform_snapshot.failed;
    out->contributions = platform_snapshot.contributions;
    out->extension_points = platform_snapshot.extension_points;
    out->catalogue_entries = platform_snapshot.catalogue_entries;
    out->pending_updates = platform_snapshot.pending_updates;
    out->audit_events = platform_snapshot.audit_events;
    out->permission_grants = platform_snapshot.permission_grants;
    out->item_count = out->installed + out->catalogue_entries;
    copy_text(out->isolation_mode, sizeof(out->isolation_mode),
              umi_plugin_isolation_mode_text(
                  platform_snapshot.default_untrusted_isolation));
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_extension_centre_installed_view(
    UmiStudioServices *services, const char *view_id, UmiUiViewModel **out_view)
{
    UmiPluginManager *manager = extension_manager(services);
    return manager != NULL
        ? umi_plugin_ui_installed_view_create(view_id, manager, out_view)
        : UMI_STATUS_UNAVAILABLE;
}

UmiStatus umi_studio_extension_centre_catalogue_view(
    UmiStudioServices *services, const char *view_id, UmiUiViewModel **out_view)
{
    UmiPluginManager *manager = extension_manager(services);
    return manager != NULL
        ? umi_plugin_ui_catalogue_view_create(view_id, manager, out_view)
        : UMI_STATUS_UNAVAILABLE;
}

UmiStatus umi_studio_extension_centre_permissions_view(
    UmiStudioServices *services, const char *view_id, UmiUiViewModel **out_view)
{
    UmiPluginManager *manager = extension_manager(services);
    return manager != NULL
        ? umi_plugin_ui_permissions_view_create(view_id, manager, out_view)
        : UMI_STATUS_UNAVAILABLE;
}

UmiStatus umi_studio_extension_centre_audit_view(
    UmiStudioServices *services, const char *view_id, UmiUiViewModel **out_view)
{
    UmiPluginManager *manager = extension_manager(services);
    return manager != NULL
        ? umi_plugin_ui_audit_view_create(view_id, manager, out_view)
        : UMI_STATUS_UNAVAILABLE;
}
