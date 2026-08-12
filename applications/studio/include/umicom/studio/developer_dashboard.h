/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/developer_dashboard.h
 *
 * PURPOSE:
 *   Define a combined Studio build, test, debug, terminal and source-control dashboard.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio composes reusable Framework capabilities here. Generic models and
 * engines stay in Framework so the same mechanisms can serve future products.
 */
#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_DEVELOPER_DASHBOARD_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_DEVELOPER_DASHBOARD_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/umicom.h"
#include "umicom/studio/services.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioDeveloperDashboardSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char area_id[128];
    char view_type[128];
    char title[256];
    char summary[512];
    uint64_t revision;
    size_t item_count;
    int available;
} UmiStudioDeveloperDashboardSnapshot;
UmiStatus umi_studio_developer_dashboard_snapshot(UmiStudioServices *services, UmiStudioDeveloperDashboardSnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif
#endif
