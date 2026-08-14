/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 clipboard coordinator
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Copy and paste retain complete semantic component subtrees. */
#ifndef UMICOM_STUDIO_VISUAL_BUILDER_CLIPBOARD_H
#define UMICOM_STUDIO_VISUAL_BUILDER_CLIPBOARD_H
#include "umicom/studio/visual_builder_centre.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_visual_builder_copy(UmiStudioVisualBuilderCentre *centre);
UmiStatus umi_studio_visual_builder_paste(UmiStudioVisualBuilderCentre *centre,const char *parent_id,const char *id_prefix);
size_t umi_studio_visual_builder_clipboard_count(UmiStudioVisualBuilderCentre *centre);
#ifdef __cplusplus
}
#endif
#endif
