/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 toolkit-neutral workbench panes
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: A frontend renders view-model properties without owning builder state. */
#ifndef UMICOM_STUDIO_VISUAL_BUILDER_VIEWS_H
#define UMICOM_STUDIO_VISUAL_BUILDER_VIEWS_H
#include "umicom/studio/visual_builder_centre.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiStudioVisualBuilderPane {
    UMI_STUDIO_VISUAL_BUILDER_PANE_CANVAS=1,
    UMI_STUDIO_VISUAL_BUILDER_PANE_HIERARCHY=2,
    UMI_STUDIO_VISUAL_BUILDER_PANE_PROPERTIES=3,
    UMI_STUDIO_VISUAL_BUILDER_PANE_BINDINGS=4,
    UMI_STUDIO_VISUAL_BUILDER_PANE_INTERACTIONS=5,
    UMI_STUDIO_VISUAL_BUILDER_PANE_TEMPLATES=6,
    UMI_STUDIO_VISUAL_BUILDER_PANE_PREVIEW=7,
    UMI_STUDIO_VISUAL_BUILDER_PANE_SOURCE=8
} UmiStudioVisualBuilderPane;
UmiStatus umi_studio_visual_builder_view_create(UmiStudioVisualBuilderCentre *centre,UmiStudioVisualBuilderPane pane,UmiUiViewModel **out_view);
const char *umi_studio_visual_builder_pane_id(UmiStudioVisualBuilderPane pane);
#ifdef __cplusplus
}
#endif
#endif
