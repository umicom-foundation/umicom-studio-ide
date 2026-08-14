/* Umicom Studio Visual Builder v2 binding test. Sammy Hegab, Umicom Foundation, MIT. */
/* BEGINNER NOTE: Bindings reference stable properties and state expressions. */
#include "umicom/studio/visual_builder_bindings.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
int main(void){UmiStudioVisualBuilderCentre *centre=NULL;UmiDesignerBinding binding={0};UmiDesignerBuilderSession *session;assert(umi_studio_visual_builder_centre_create("org.umicom.bindings",&centre)==UMI_STATUS_OK);session=umi_studio_visual_builder_centre_session(centre);assert(umi_designer_builder_session_add_component(session,"title","label","root")==UMI_STATUS_OK);strcpy(binding.binding_id,"b1");strcpy(binding.node_id,"title");strcpy(binding.property_name,"title");strcpy(binding.source_expression,"state.title");binding.mode=UMI_DESIGNER_BIND_ONE_WAY;binding.enabled=1;assert(umi_studio_visual_builder_binding_upsert(centre,&binding)==UMI_STATUS_OK);assert(umi_studio_visual_builder_binding_count(centre)==1U);umi_studio_visual_builder_centre_destroy(centre);return EXIT_SUCCESS;}
