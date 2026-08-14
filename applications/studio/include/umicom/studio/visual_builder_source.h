/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 transparent source adapter
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Every generated file remains visible for review, saving and diffing. */
#ifndef UMICOM_STUDIO_VISUAL_BUILDER_SOURCE_H
#define UMICOM_STUDIO_VISUAL_BUILDER_SOURCE_H
#include "umicom/studio/visual_builder_centre.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_visual_builder_source_refresh(UmiStudioVisualBuilderCentre *centre);
const UmiDesignerGeneratedFile *umi_studio_visual_builder_source_find(UmiStudioVisualBuilderCentre *centre,const char *path);
size_t umi_studio_visual_builder_source_count(UmiStudioVisualBuilderCentre *centre);
#ifdef __cplusplus
}
#endif
#endif
