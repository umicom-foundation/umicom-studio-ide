/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Application Builder v2 composition centre
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/visual_builder_centre.h"
#include <stdlib.h>
#include <string.h>
struct UmiStudioVisualBuilderCentre { UmiDesignerBuilderSession *session; char active_pane[UMI_DECL_ID_CAPACITY]; uint64_t revision; };
const char *umi_studio_visual_builder_capability_id(void){return "umicom.studio.visual-application-builder";}
UmiStatus umi_studio_visual_builder_centre_create(const char *application_id,UmiStudioVisualBuilderCentre **out_centre)
{
    UmiStudioVisualBuilderCentre *centre; UmiStatus status;
    if(application_id==NULL||out_centre==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *out_centre=NULL;
    centre=calloc(1U,sizeof(*centre)); if(centre==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    status=umi_designer_builder_session_create(application_id,&centre->session);
    if(status==UMI_STATUS_OK)status=umi_decl_copy_text(centre->active_pane,sizeof(centre->active_pane),"canvas");
    if(status!=UMI_STATUS_OK){umi_designer_builder_session_destroy(centre->session);free(centre);return status;}
    centre->revision=1U; *out_centre=centre; return UMI_STATUS_OK;
}
void umi_studio_visual_builder_centre_destroy(UmiStudioVisualBuilderCentre *centre){if(centre==NULL)return;umi_designer_builder_session_destroy(centre->session);free(centre);}
UmiDesignerBuilderSession *umi_studio_visual_builder_centre_session(UmiStudioVisualBuilderCentre *centre){return centre!=NULL?centre->session:NULL;}
UmiStatus umi_studio_visual_builder_centre_activate(UmiStudioVisualBuilderCentre *centre,const char *pane_id){UmiStatus status;if(centre==NULL||pane_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;status=umi_decl_copy_text(centre->active_pane,sizeof(centre->active_pane),pane_id);if(status==UMI_STATUS_OK)centre->revision+=1U;return status;}
UmiStatus umi_studio_visual_builder_centre_snapshot(const UmiStudioVisualBuilderCentre *centre,UmiStudioVisualBuilderSnapshot *out_snapshot){UmiStatus status;if(centre==NULL||out_snapshot==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out_snapshot,0,sizeof(*out_snapshot));status=umi_designer_builder_session_snapshot(centre->session,&out_snapshot->builder);if(status==UMI_STATUS_OK)status=umi_decl_copy_text(out_snapshot->active_pane,sizeof(out_snapshot->active_pane),centre->active_pane);out_snapshot->revision=centre->revision;return status;}
