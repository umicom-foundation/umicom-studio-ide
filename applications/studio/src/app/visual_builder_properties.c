/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 typed property inspector
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/visual_builder_properties.h"
UmiStatus umi_studio_visual_builder_property_begin(UmiStudioVisualBuilderCentre *centre,const char *node_id,const char *property_name,UmiDesignerPropertyDraft *out_draft){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);if(session==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_designer_property_editor_begin(umi_designer_builder_session_document(session),umi_designer_builder_session_components(session),node_id,property_name,out_draft);}
UmiStatus umi_studio_visual_builder_property_set(UmiDesignerPropertyDraft *draft,const char *value_text){return umi_designer_property_editor_set(draft,value_text);}
UmiStatus umi_studio_visual_builder_property_commit(UmiStudioVisualBuilderCentre *centre,const UmiDesignerPropertyDraft *draft){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);if(session==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_designer_property_editor_commit(umi_designer_builder_session_history(session),umi_designer_builder_session_document(session),draft);}
