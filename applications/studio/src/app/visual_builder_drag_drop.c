/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 drag/drop coordinator
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/visual_builder_drag_drop.h"
UmiStatus umi_studio_visual_builder_drag_palette(UmiDesignerDragDropV2 *drag,const char *component_type,const char *node_id){return umi_designer_drag_drop_v2_begin_palette(drag,component_type,node_id);}
UmiStatus umi_studio_visual_builder_drag_node(UmiDesignerDragDropV2 *drag,const char *node_id){return umi_designer_drag_drop_v2_begin_node(drag,node_id);}
UmiStatus umi_studio_visual_builder_drag_target(UmiDesignerDragDropV2 *drag,const char *target_id,UmiDesignerDropPositionV2 position){return umi_designer_drag_drop_v2_target(drag,target_id,position);}
UmiStatus umi_studio_visual_builder_drop(UmiStudioVisualBuilderCentre *centre,UmiDesignerDragDropV2 *drag){UmiDesignerBuilderSessionV2 *session=umi_studio_visual_builder_centre_session(centre);if(session==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_designer_drag_drop_v2_commit(drag,umi_designer_builder_session_v2_history(session),umi_designer_builder_session_v2_document(session));}
