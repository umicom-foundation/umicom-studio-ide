/* Umicom Studio Visual Builder v2 interaction test. Sammy Hegab, Umicom Foundation, MIT. */
/* BEGINNER NOTE: Named command mappings are portable across frontends. */
#include "umicom/studio/visual_builder_interactions.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
int main(void){UmiStudioVisualBuilderCentre *centre=NULL;UmiDesignerInteraction item={0};UmiDesignerBuilderSession *session;assert(umi_studio_visual_builder_centre_create("org.umicom.interactions",&centre)==UMI_STATUS_OK);session=umi_studio_visual_builder_centre_session(centre);assert(umi_designer_builder_session_add_component(session,"save","button","root")==UMI_STATUS_OK);strcpy(item.interaction_id,"i1");strcpy(item.node_id,"save");strcpy(item.source_name,"clicked");strcpy(item.command_id,"document.save");item.kind=UMI_DESIGNER_INTERACTION_COMMAND;item.enabled=1;assert(umi_studio_visual_builder_interaction_upsert(centre,&item)==UMI_STATUS_OK);assert(umi_studio_visual_builder_interaction_count(centre)==1U);umi_studio_visual_builder_centre_destroy(centre);return EXIT_SUCCESS;}
