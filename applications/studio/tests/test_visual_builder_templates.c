/* Umicom Studio Visual Builder v2 template test. Sammy Hegab, Umicom Foundation, MIT. */
/* BEGINNER NOTE: Palette templates instantiate visible normal components. */
#include "umicom/studio/visual_builder_templates.h"
#include <assert.h>
#include <stdlib.h>
int main(void){UmiStudioVisualBuilderCentre *centre=NULL;UmiDesignerTemplate item;assert(umi_studio_visual_builder_centre_create("org.umicom.templates",&centre)==UMI_STATUS_OK);assert(umi_studio_visual_builder_template_count(centre)>0U);assert(umi_studio_visual_builder_template_find(centre,"form-card",&item)==UMI_STATUS_OK);assert(umi_studio_visual_builder_template_apply(centre,"form-card","root","customer")==UMI_STATUS_OK);umi_studio_visual_builder_centre_destroy(centre);return EXIT_SUCCESS;}
