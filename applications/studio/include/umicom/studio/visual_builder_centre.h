/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/visual_builder_centre.h
 * PURPOSE: Own Studio's Framework-backed Visual Application Builder v2 centre.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Studio owns product composition; reusable authoring state lives in Framework. */
#ifndef UMICOM_STUDIO_VISUAL_BUILDER_CENTRE_H
#define UMICOM_STUDIO_VISUAL_BUILDER_CENTRE_H
#include "umicom/umicom.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioVisualBuilderCentre UmiStudioVisualBuilderCentre;
typedef struct UmiStudioVisualBuilderSnapshot {
    UmiDesignerBuilderSessionSnapshot builder;
    char active_pane[UMI_DECL_ID_CAPACITY];
    uint64_t revision;
} UmiStudioVisualBuilderSnapshot;
UmiStatus umi_studio_visual_builder_centre_create(const char *application_id,UmiStudioVisualBuilderCentre **out_centre);
void umi_studio_visual_builder_centre_destroy(UmiStudioVisualBuilderCentre *centre);
UmiDesignerBuilderSession *umi_studio_visual_builder_centre_session(UmiStudioVisualBuilderCentre *centre);
UmiStatus umi_studio_visual_builder_centre_activate(UmiStudioVisualBuilderCentre *centre,const char *pane_id);
UmiStatus umi_studio_visual_builder_centre_snapshot(const UmiStudioVisualBuilderCentre *centre,UmiStudioVisualBuilderSnapshot *out_snapshot);
const char *umi_studio_visual_builder_capability_id(void);
#ifdef __cplusplus
}
#endif
#endif
