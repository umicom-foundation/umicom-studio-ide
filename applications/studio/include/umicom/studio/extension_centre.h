/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/extension_centre.h
 *
 * PURPOSE:
 *   Define Studio extension-centre composition over plug-in and marketplace contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio composes reusable Framework capabilities here. Generic models and
 * engines stay in Framework so the same mechanisms can serve future products.
 */
#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_EXTENSION_CENTRE_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_EXTENSION_CENTRE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/umicom.h"
#include "umicom/studio/services.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioExtensionCentreSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char area_id[128];
    char view_type[128];
    char title[256];
    char summary[512];
    uint64_t revision;
    size_t item_count;
    size_t installed;
    size_t enabled;
    size_t active;
    size_t failed;
    size_t contributions;
    size_t extension_points;
    size_t catalogue_entries;
    size_t pending_updates;
    size_t audit_events;
    size_t permission_grants;
    char isolation_mode[64];
    int available;
} UmiStudioExtensionCentreSnapshot;
UmiStatus umi_studio_extension_centre_snapshot(UmiStudioServices *services, UmiStudioExtensionCentreSnapshot *out_snapshot);
UmiStatus umi_studio_extension_centre_installed_view(
    UmiStudioServices *services, const char *view_id, UmiUiViewModel **out_view);
UmiStatus umi_studio_extension_centre_catalogue_view(
    UmiStudioServices *services, const char *view_id, UmiUiViewModel **out_view);
UmiStatus umi_studio_extension_centre_permissions_view(
    UmiStudioServices *services, const char *view_id, UmiUiViewModel **out_view);
UmiStatus umi_studio_extension_centre_audit_view(
    UmiStudioServices *services, const char *view_id, UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
