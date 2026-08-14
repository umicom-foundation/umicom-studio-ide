/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/distribution_views_v2.c
 *
 * PURPOSE:
 *   Catalogue the views shown by Studio's SDK and Distribution Centre.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/distribution_views_v2.h"
#include <string.h>

void umi_studio_distribution_views_v2_init(UmiStudioDistributionViewsV2 *views)
{
    static const UmiStudioDistributionViewV2 defaults[] = {
        {"sdk.exports", "SDK Exports", "left"},
        {"packages.manifest", "Package Manifest", "centre"},
        {"packages.dependencies", "Runtime Dependencies", "bottom"},
        {"packages.bundle", "Bundle Designer", "centre"},
        {"installers", "Installer Centre", "centre"},
        {"supply-chain", "Supply Chain", "bottom"},
        {"release.channels", "Release Channels", "right"},
        {"updates", "Updates and Rollback", "right"}
    };
    size_t index;
    if (views == NULL) return;
    (void)memset(views, 0, sizeof(*views));
    for (index = 0U; index < sizeof(defaults) / sizeof(defaults[0]); ++index) {
        views->views[index] = defaults[index];
    }
    views->count = sizeof(defaults) / sizeof(defaults[0]);
}

const UmiStudioDistributionViewV2 *umi_studio_distribution_views_v2_find(
    const UmiStudioDistributionViewsV2 *views,
    const char *view_id)
{
    size_t index;
    if (views == NULL || view_id == NULL) return NULL;
    for (index = 0U; index < views->count; ++index) {
        if (strcmp(views->views[index].view_id, view_id) == 0) {
            return &views->views[index];
        }
    }
    return NULL;
}
