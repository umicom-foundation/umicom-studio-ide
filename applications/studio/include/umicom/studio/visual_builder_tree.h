/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 component hierarchy adapter
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: The displayed tree is rebuilt from the Framework semantic document. */
#ifndef UMICOM_STUDIO_VISUAL_BUILDER_TREE_H
#define UMICOM_STUDIO_VISUAL_BUILDER_TREE_H
#include "umicom/studio/visual_builder_centre.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_visual_builder_tree_refresh(UmiStudioVisualBuilderCentre *centre,UmiDesignerComponentTree *out_tree);
UmiStatus umi_studio_visual_builder_tree_select(UmiStudioVisualBuilderCentre *centre,const char *node_id,int extend_selection);
UmiStatus umi_studio_visual_builder_tree_reparent(UmiStudioVisualBuilderCentre *centre,const char *node_id,const char *parent_id,uint32_t sibling_index);
#ifdef __cplusplus
}
#endif
#endif
