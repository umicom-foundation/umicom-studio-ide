/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/distribution_views.h
 *
 * PURPOSE:
 *   Catalogue the views shown by Studio's SDK and Distribution Centre.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_DISTRIBUTION_VIEWS_H
#define UMICOM_STUDIO_DISTRIBUTION_VIEWS_H

#include <stddef.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_DISTRIBUTION_MAX_VIEWS 8U

typedef struct UmiStudioDistributionView {
    const char *view_id;
    const char *title;
    const char *region;
} UmiStudioDistributionView;

typedef struct UmiStudioDistributionViews {
    UmiStudioDistributionView views[UMI_STUDIO_DISTRIBUTION_MAX_VIEWS];
    size_t count;
} UmiStudioDistributionViews;

void umi_studio_distribution_views_init(UmiStudioDistributionViews *views);
const UmiStudioDistributionView *umi_studio_distribution_views_find(
    const UmiStudioDistributionViews *views,
    const char *view_id);

#ifdef __cplusplus
}
#endif
#endif
