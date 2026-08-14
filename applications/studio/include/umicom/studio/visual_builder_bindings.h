/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 property-binding editor
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Studio edits stable Framework binding records, not hidden widget state. */
#ifndef UMICOM_STUDIO_VISUAL_BUILDER_BINDINGS_H
#define UMICOM_STUDIO_VISUAL_BUILDER_BINDINGS_H
#include "umicom/studio/visual_builder_centre.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_visual_builder_binding_upsert(UmiStudioVisualBuilderCentre *centre,const UmiDesignerBinding *binding);
UmiStatus umi_studio_visual_builder_binding_remove(UmiStudioVisualBuilderCentre *centre,const char *binding_id);
UmiStatus umi_studio_visual_builder_binding_at(UmiStudioVisualBuilderCentre *centre,size_t index,UmiDesignerBinding *out_binding);
size_t umi_studio_visual_builder_binding_count(UmiStudioVisualBuilderCentre *centre);
#ifdef __cplusplus
}
#endif
#endif
