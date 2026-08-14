/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 live-preview adapter
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Preview health exposes current, stale and invalid states honestly. */
#ifndef UMICOM_STUDIO_VISUAL_BUILDER_PREVIEW_H
#define UMICOM_STUDIO_VISUAL_BUILDER_PREVIEW_H
#include "umicom/studio/visual_builder_centre.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_visual_builder_preview_refresh(UmiStudioVisualBuilderCentre *centre);
UmiStatus umi_studio_visual_builder_preview_profile(UmiStudioVisualBuilderCentre *centre,UmiDesignerPreviewProfile profile);
const UmiDesignerLivePreview *umi_studio_visual_builder_preview_state(UmiStudioVisualBuilderCentre *centre);
#ifdef __cplusplus
}
#endif
#endif
