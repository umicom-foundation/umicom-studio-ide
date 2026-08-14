/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 drag/drop coordinator
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Frontends report a completed semantic drop; Framework owns the edit. */
#ifndef UMICOM_STUDIO_VISUAL_BUILDER_DRAG_DROP_H
#define UMICOM_STUDIO_VISUAL_BUILDER_DRAG_DROP_H
#include "umicom/studio/visual_builder_centre.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_visual_builder_drag_palette(UmiDesignerDragDropV2 *drag,const char *component_type,const char *node_id);
UmiStatus umi_studio_visual_builder_drag_node(UmiDesignerDragDropV2 *drag,const char *node_id);
UmiStatus umi_studio_visual_builder_drag_target(UmiDesignerDragDropV2 *drag,const char *target_id,UmiDesignerDropPositionV2 position);
UmiStatus umi_studio_visual_builder_drop(UmiStudioVisualBuilderCentre *centre,UmiDesignerDragDropV2 *drag);
#ifdef __cplusplus
}
#endif
#endif
