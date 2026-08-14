/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/visual_builder_main.c
 * PURPOSE: Inspect the native Visual Application Builder v2 composition.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: This command exercises the same services used by graphical frontends. */
#include "umicom/studio/visual_builder_centre.h"
#include "umicom/studio/visual_builder_preview.h"
#include "umicom/studio/visual_builder_source.h"
#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    UmiStudioVisualBuilderCentre *centre=NULL; UmiStudioVisualBuilderSnapshot snapshot; UmiDesignerBuilderSession *session;
    if(umi_studio_visual_builder_centre_create("org.umicom.studio.visual-builder",&centre)!=UMI_STATUS_OK)return EXIT_FAILURE;
    session=umi_studio_visual_builder_centre_session(centre);
    if(umi_designer_builder_session_add_component(session,"welcome","label","root")!=UMI_STATUS_OK||umi_studio_visual_builder_preview_refresh(centre)!=UMI_STATUS_OK||umi_studio_visual_builder_source_refresh(centre)!=UMI_STATUS_OK||umi_studio_visual_builder_centre_snapshot(centre,&snapshot)!=UMI_STATUS_OK){umi_studio_visual_builder_centre_destroy(centre);return EXIT_FAILURE;}
    (void)printf("capability=%s components=%zu generated=%zu preview=%s\n",umi_studio_visual_builder_capability_id(),snapshot.builder.document.component_count,umi_studio_visual_builder_source_count(centre),umi_designer_preview_health_text(snapshot.builder.preview_health));
    umi_studio_visual_builder_centre_destroy(centre);return EXIT_SUCCESS;
}
