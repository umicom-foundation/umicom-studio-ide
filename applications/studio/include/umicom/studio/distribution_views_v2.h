/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/distribution_views_v2.h
 *
 * PURPOSE:
 *   Catalogue the views shown by Studio's SDK and Distribution Centre.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_DISTRIBUTION_VIEWS_V2_H
#define UMICOM_STUDIO_DISTRIBUTION_VIEWS_V2_H

#include <stddef.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_DISTRIBUTION_MAX_VIEWS_V2 8U

typedef struct UmiStudioDistributionViewV2 {
    const char *view_id;
    const char *title;
    const char *region;
} UmiStudioDistributionViewV2;

typedef struct UmiStudioDistributionViewsV2 {
    UmiStudioDistributionViewV2 views[UMI_STUDIO_DISTRIBUTION_MAX_VIEWS_V2];
    size_t count;
} UmiStudioDistributionViewsV2;

void umi_studio_distribution_views_v2_init(UmiStudioDistributionViewsV2 *views);
const UmiStudioDistributionViewV2 *umi_studio_distribution_views_v2_find(
    const UmiStudioDistributionViewsV2 *views,
    const char *view_id);

#ifdef __cplusplus
}
#endif
#endif
