/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 component hierarchy adapter
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/visual_builder_tree.h"
UmiStatus umi_studio_visual_builder_tree_refresh(UmiStudioVisualBuilderCentre *centre,UmiDesignerComponentTree *out_tree){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);return session!=NULL?umi_designer_builder_session_tree(session,out_tree):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_studio_visual_builder_tree_select(UmiStudioVisualBuilderCentre *centre,const char *node_id,int extend_selection){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);return session!=NULL?umi_designer_builder_session_select(session,node_id,extend_selection):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_studio_visual_builder_tree_reparent(UmiStudioVisualBuilderCentre *centre,const char *node_id,const char *parent_id,uint32_t sibling_index){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);if(session==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_designer_component_tree_reparent(umi_designer_builder_session_history(session),umi_designer_builder_session_document(session),node_id,parent_id,sibling_index);}
