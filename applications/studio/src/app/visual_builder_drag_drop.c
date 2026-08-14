/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 drag/drop coordinator
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/visual_builder_drag_drop.h"
UmiStatus umi_studio_visual_builder_drag_palette(UmiDesignerDragDrop *drag,const char *component_type,const char *node_id){return umi_designer_drag_drop_begin_palette(drag,component_type,node_id);}
UmiStatus umi_studio_visual_builder_drag_node(UmiDesignerDragDrop *drag,const char *node_id){return umi_designer_drag_drop_begin_node(drag,node_id);}
UmiStatus umi_studio_visual_builder_drag_target(UmiDesignerDragDrop *drag,const char *target_id,UmiDesignerDropPosition position){return umi_designer_drag_drop_target(drag,target_id,position);}
UmiStatus umi_studio_visual_builder_drop(UmiStudioVisualBuilderCentre *centre,UmiDesignerDragDrop *drag){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);if(session==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_designer_drag_drop_commit(drag,umi_designer_builder_session_history(session),umi_designer_builder_session_document(session));}
