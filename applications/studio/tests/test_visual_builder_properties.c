/* Umicom Studio Visual Builder v2 property test. Sammy Hegab, Umicom Foundation, MIT. */
/* BEGINNER NOTE: Studio commits only validated typed drafts. */
#include "umicom/studio/visual_builder_properties.h"
#include <assert.h>
#include <stdlib.h>
int main(void){UmiStudioVisualBuilderCentre *centre=NULL;UmiDesignerPropertyDraft draft;UmiDesignerBuilderSession *session;assert(umi_studio_visual_builder_centre_create("org.umicom.properties",&centre)==UMI_STATUS_OK);session=umi_studio_visual_builder_centre_session(centre);assert(umi_designer_builder_session_add_component(session,"save","button","root")==UMI_STATUS_OK);assert(umi_studio_visual_builder_property_begin(centre,"save","title",&draft)==UMI_STATUS_OK);assert(umi_studio_visual_builder_property_set(&draft,"Save")==UMI_STATUS_OK);assert(umi_studio_visual_builder_property_commit(centre,&draft)==UMI_STATUS_OK);umi_studio_visual_builder_centre_destroy(centre);return EXIT_SUCCESS;}
