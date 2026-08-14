/* Umicom Studio Visual Builder v2 tree test. Sammy Hegab, Umicom Foundation, MIT. */
/* BEGINNER NOTE: The hierarchy pane projects semantic parent relationships. */
#include "umicom/studio/visual_builder_tree.h"
#include <assert.h>
#include <stdlib.h>
int main(void){UmiStudioVisualBuilderCentre *centre=NULL;UmiDesignerComponentTree tree;UmiDesignerBuilderSession *session;assert(umi_studio_visual_builder_centre_create("org.umicom.tree",&centre)==UMI_STATUS_OK);session=umi_studio_visual_builder_centre_session(centre);assert(umi_designer_builder_session_add_component(session,"panel","pane","root")==UMI_STATUS_OK);assert(umi_studio_visual_builder_tree_select(centre,"panel",0)==UMI_STATUS_OK);assert(umi_studio_visual_builder_tree_refresh(centre,&tree)==UMI_STATUS_OK);assert(umi_designer_component_tree_find(&tree,"panel")->selected!=0);umi_studio_visual_builder_centre_destroy(centre);return EXIT_SUCCESS;}
