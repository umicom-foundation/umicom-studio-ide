/* Umicom Studio Visual Builder v2 centre test. Sammy Hegab, Umicom Foundation, MIT. */
/* BEGINNER NOTE: The centre exposes one Framework-owned authoring session. */
#include "umicom/studio/visual_builder_centre.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
int main(void){UmiStudioVisualBuilderCentre *centre=NULL;UmiStudioVisualBuilderSnapshot snapshot;assert(umi_studio_visual_builder_centre_create("org.umicom.centre",&centre)==UMI_STATUS_OK);assert(strcmp(umi_studio_visual_builder_capability_id(),"umicom.studio.visual-application-builder")==0);assert(umi_studio_visual_builder_centre_activate(centre,"hierarchy")==UMI_STATUS_OK);assert(umi_studio_visual_builder_centre_snapshot(centre,&snapshot)==UMI_STATUS_OK);assert(snapshot.builder.document.component_count==1U);assert(strcmp(snapshot.active_pane,"hierarchy")==0);umi_studio_visual_builder_centre_destroy(centre);return EXIT_SUCCESS;}
