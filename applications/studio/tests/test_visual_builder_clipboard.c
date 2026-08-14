/* Umicom Studio Visual Builder v2 clipboard test. Sammy Hegab, Umicom Foundation, MIT. */
/* BEGINNER NOTE: Paste remaps identities while preserving subtree relationships. */
#include "umicom/studio/visual_builder_clipboard.h"
#include "umicom/studio/visual_builder_tree.h"
#include <assert.h>
#include <stdlib.h>
int main(void){UmiStudioVisualBuilderCentre *centre=NULL;UmiDesignerBuilderSession *session;assert(umi_studio_visual_builder_centre_create("org.umicom.clip",&centre)==UMI_STATUS_OK);session=umi_studio_visual_builder_centre_session(centre);assert(umi_designer_builder_session_add_component(session,"card","pane","root")==UMI_STATUS_OK);assert(umi_studio_visual_builder_tree_select(centre,"card",0)==UMI_STATUS_OK);assert(umi_studio_visual_builder_copy(centre)==UMI_STATUS_OK);assert(umi_studio_visual_builder_clipboard_count(centre)==1U);assert(umi_studio_visual_builder_paste(centre,"root","copy")==UMI_STATUS_OK);umi_studio_visual_builder_centre_destroy(centre);return EXIT_SUCCESS;}
