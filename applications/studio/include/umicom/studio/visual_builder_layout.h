/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 layout command adapter
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Layout commands are atomic and operate on the current semantic selection. */
#ifndef UMICOM_STUDIO_VISUAL_BUILDER_LAYOUT_H
#define UMICOM_STUDIO_VISUAL_BUILDER_LAYOUT_H
#include "umicom/studio/visual_builder_centre.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_visual_builder_align(UmiStudioVisualBuilderCentre *centre,UmiDesignerAlignment alignment);
UmiStatus umi_studio_visual_builder_distribute(UmiStudioVisualBuilderCentre *centre,UmiDesignerDistribution distribution);
UmiStatus umi_studio_visual_builder_snap(UmiStudioVisualBuilderCentre *centre,const UmiDesignerSurfaceOptions *options);
#ifdef __cplusplus
}
#endif
#endif
