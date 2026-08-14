/* Umicom Studio Visual Builder v2 preview/source test. Sammy Hegab, Umicom Foundation, MIT. */
/* BEGINNER NOTE: Preview and visible source share the semantic document revision. */
#include "umicom/studio/visual_builder_preview.h"
#include "umicom/studio/visual_builder_source.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
int main(void){UmiStudioVisualBuilderCentre *centre=NULL;const UmiDesignerGeneratedFile *file;assert(umi_studio_visual_builder_centre_create("org.umicom.preview",&centre)==UMI_STATUS_OK);assert(umi_studio_visual_builder_preview_profile(centre,UMI_DESIGNER_PREVIEW_PHONE_PORTRAIT)==UMI_STATUS_OK);assert(umi_studio_visual_builder_preview_refresh(centre)==UMI_STATUS_OK);assert(umi_studio_visual_builder_source_refresh(centre)==UMI_STATUS_OK);assert(umi_studio_visual_builder_source_count(centre)==4U);file=umi_studio_visual_builder_source_find(centre,"application.umiapp");assert(file!=NULL&&strstr(file->content,"org.umicom.preview")!=NULL);umi_studio_visual_builder_centre_destroy(centre);return EXIT_SUCCESS;}
