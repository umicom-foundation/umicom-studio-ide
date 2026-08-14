/* Umicom Studio Visual Builder v2 drag/drop test. Sammy Hegab, Umicom Foundation, MIT. */
/* BEGINNER NOTE: Completed drops become undoable semantic edits. */
#include "umicom/studio/visual_builder_drag_drop.h"
#include <assert.h>
#include <stdlib.h>
int main(void){UmiStudioVisualBuilderCentre *centre=NULL;UmiDesignerDragDrop drag;assert(umi_studio_visual_builder_centre_create("org.umicom.drop",&centre)==UMI_STATUS_OK);assert(umi_studio_visual_builder_drag_palette(&drag,"button","save")==UMI_STATUS_OK);assert(umi_studio_visual_builder_drag_target(&drag,"root",UMI_DESIGNER_DROP_INTO)==UMI_STATUS_OK);assert(umi_studio_visual_builder_drop(centre,&drag)==UMI_STATUS_OK);umi_studio_visual_builder_centre_destroy(centre);return EXIT_SUCCESS;}
