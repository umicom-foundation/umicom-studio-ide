/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/chart_workspace.h
 *
 * PURPOSE:
 *   Define Studio chart workspace for reusable Framework chart and trading components.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio composes reusable Framework capabilities here. Generic models and
 * engines stay in Framework so the same mechanisms can serve future products.
 */
#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_CHART_WORKSPACE_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_CHART_WORKSPACE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/umicom.h"
#include "umicom/studio/services.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioChartWorkspaceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char area_id[128];
    char view_type[128];
    char title[256];
    char summary[512];
    uint64_t revision;
    size_t item_count;
    int available;
} UmiStudioChartWorkspaceSnapshot;
UmiStatus umi_studio_chart_workspace_snapshot(UmiStudioServices *services, UmiStudioChartWorkspaceSnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif
#endif
