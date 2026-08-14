/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 property-binding editor
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/visual_builder_bindings.h"
UmiStatus umi_studio_visual_builder_binding_upsert(UmiStudioVisualBuilderCentre *centre,const UmiDesignerBinding *binding){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);if(session==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_designer_binding_editor_upsert(umi_designer_builder_session_bindings(session),umi_designer_builder_session_document(session),umi_designer_builder_session_components(session),binding);}
UmiStatus umi_studio_visual_builder_binding_remove(UmiStudioVisualBuilderCentre *centre,const char *binding_id){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);return session!=NULL?umi_designer_binding_editor_remove(umi_designer_builder_session_bindings(session),binding_id):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_studio_visual_builder_binding_at(UmiStudioVisualBuilderCentre *centre,size_t index,UmiDesignerBinding *out_binding){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);return session!=NULL?umi_designer_binding_editor_at(umi_designer_builder_session_bindings(session),index,out_binding):UMI_STATUS_INVALID_ARGUMENT;}
size_t umi_studio_visual_builder_binding_count(UmiStudioVisualBuilderCentre *centre){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);return session!=NULL?umi_designer_binding_editor_count(umi_designer_builder_session_bindings(session)):0U;}
