/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 clipboard coordinator
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/visual_builder_clipboard.h"
UmiStatus umi_studio_visual_builder_copy(UmiStudioVisualBuilderCentre *centre){UmiDesignerBuilderSessionV2 *session=umi_studio_visual_builder_centre_session(centre);if(session==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_designer_clipboard_v2_copy(umi_designer_builder_session_v2_clipboard(session),umi_designer_builder_session_v2_document(session),umi_designer_builder_session_v2_selection(session));}
UmiStatus umi_studio_visual_builder_paste(UmiStudioVisualBuilderCentre *centre,const char *parent_id,const char *id_prefix){UmiDesignerBuilderSessionV2 *session=umi_studio_visual_builder_centre_session(centre);if(session==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_designer_clipboard_v2_paste(umi_designer_builder_session_v2_clipboard(session),umi_designer_builder_session_v2_document(session),umi_designer_builder_session_v2_transactions(session),parent_id,id_prefix);}
size_t umi_studio_visual_builder_clipboard_count(UmiStudioVisualBuilderCentre *centre){UmiDesignerBuilderSessionV2 *session=umi_studio_visual_builder_centre_session(centre);UmiDesignerClipboardV2 *clipboard=session!=NULL?umi_designer_builder_session_v2_clipboard(session):NULL;return clipboard!=NULL?clipboard->node_count:0U;}
