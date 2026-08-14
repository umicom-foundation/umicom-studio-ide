/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 live-preview adapter
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/visual_builder_preview.h"
UmiStatus umi_studio_visual_builder_preview_refresh(UmiStudioVisualBuilderCentre *centre){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);return session!=NULL?umi_designer_builder_session_preview(session):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_studio_visual_builder_preview_profile(UmiStudioVisualBuilderCentre *centre,UmiDesignerPreviewProfile profile){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);UmiDesignerLivePreview *preview=session!=NULL?umi_designer_builder_session_live_preview(session):NULL;return preview!=NULL?umi_designer_preview_apply_profile(&preview->viewport,profile):UMI_STATUS_INVALID_ARGUMENT;}
const UmiDesignerLivePreview *umi_studio_visual_builder_preview_state(UmiStudioVisualBuilderCentre *centre){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);return session!=NULL?umi_designer_builder_session_live_preview(session):NULL;}
