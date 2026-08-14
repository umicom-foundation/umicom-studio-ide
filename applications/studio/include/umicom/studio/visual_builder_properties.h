/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 typed property inspector
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Property drafts validate against component schemas before commit. */
#ifndef UMICOM_STUDIO_VISUAL_BUILDER_PROPERTIES_H
#define UMICOM_STUDIO_VISUAL_BUILDER_PROPERTIES_H
#include "umicom/studio/visual_builder_centre.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_visual_builder_property_begin(UmiStudioVisualBuilderCentre *centre,const char *node_id,const char *property_name,UmiDesignerPropertyDraft *out_draft);
UmiStatus umi_studio_visual_builder_property_set(UmiDesignerPropertyDraft *draft,const char *value_text);
UmiStatus umi_studio_visual_builder_property_commit(UmiStudioVisualBuilderCentre *centre,const UmiDesignerPropertyDraft *draft);
#ifdef __cplusplus
}
#endif
#endif
