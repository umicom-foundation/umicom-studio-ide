/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/navigation_view_contribution.h
 *
 * PURPOSE:
 *   Define Studio placement for Framework-owned source insight models without
 *   duplicating navigation, grouping or hierarchy behaviour in the product.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_NAVIGATION_VIEW_CONTRIBUTION_H
#define UMICOM_STUDIO_NAVIGATION_VIEW_CONTRIBUTION_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_NAVIGATION_VIEW_CONTRIBUTION_API_VERSION 1U

typedef struct UmiStudioNavigationViewContribution {
    uint32_t struct_size;
    uint32_t api_version;
    const char *view_id;
    const char *title;
    const char *framework_contract;
    const char *default_region;
    int32_t order;
    int closable;
    int movable;
} UmiStudioNavigationViewContribution;

size_t umi_studio_navigation_view_contribution_count(void);
const UmiStudioNavigationViewContribution *
umi_studio_navigation_view_contribution_at(size_t index);
const UmiStudioNavigationViewContribution *
umi_studio_navigation_view_contribution_find(const char *view_id);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_STUDIO_NAVIGATION_VIEW_CONTRIBUTION_H */
