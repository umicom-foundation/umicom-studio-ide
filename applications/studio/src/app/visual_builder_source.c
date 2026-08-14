/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 transparent source adapter
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/visual_builder_source.h"
UmiStatus umi_studio_visual_builder_source_refresh(UmiStudioVisualBuilderCentre *centre){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);return session!=NULL?umi_designer_builder_session_generate(session):UMI_STATUS_INVALID_ARGUMENT;}
const UmiDesignerGeneratedFile *umi_studio_visual_builder_source_find(UmiStudioVisualBuilderCentre *centre,const char *path){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);return session!=NULL?umi_designer_source_generation_find(umi_designer_builder_session_generated_source(session),path):NULL;}
size_t umi_studio_visual_builder_source_count(UmiStudioVisualBuilderCentre *centre){UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre);const UmiDesignerGeneratedSource *source=session!=NULL?umi_designer_builder_session_generated_source(session):NULL;return source!=NULL?source->file_count:0U;}
