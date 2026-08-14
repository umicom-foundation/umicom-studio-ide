/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 clipboard coordinator
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/visual_builder_clipboard.h"
UmiStatus umi_studio_visual_builder_copy(UmiStudioVisualBuilderCentre *centre){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);if(session==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_designer_clipboard_copy(umi_designer_builder_session_clipboard(session),umi_designer_builder_session_document(session),umi_designer_builder_session_selection(session));}
UmiStatus umi_studio_visual_builder_paste(UmiStudioVisualBuilderCentre *centre,const char *parent_id,const char *id_prefix){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);if(session==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_designer_clipboard_paste(umi_designer_builder_session_clipboard(session),umi_designer_builder_session_document(session),umi_designer_builder_session_transactions(session),parent_id,id_prefix);}
size_t umi_studio_visual_builder_clipboard_count(UmiStudioVisualBuilderCentre *centre){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);UmiDesignerSemanticClipboard *clipboard=session!=NULL?umi_designer_builder_session_clipboard(session):NULL;return clipboard!=NULL?clipboard->node_count:0U;}
