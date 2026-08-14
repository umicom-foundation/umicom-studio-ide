/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 template palette coordinator
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Templates expand to normal document nodes in one transaction. */
#ifndef UMICOM_STUDIO_VISUAL_BUILDER_TEMPLATES_H
#define UMICOM_STUDIO_VISUAL_BUILDER_TEMPLATES_H
#include "umicom/studio/visual_builder_centre.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_visual_builder_template_find(UmiStudioVisualBuilderCentre *centre,const char *template_id,UmiDesignerTemplate *out_template);
UmiStatus umi_studio_visual_builder_template_apply(UmiStudioVisualBuilderCentre *centre,const char *template_id,const char *parent_id,const char *id_prefix);
size_t umi_studio_visual_builder_template_count(UmiStudioVisualBuilderCentre *centre);
#ifdef __cplusplus
}
#endif
#endif
