/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 toolkit-neutral workbench panes
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/visual_builder_views.h"
#include <stdint.h>
static UmiStatus set_text(UmiUiViewModel *view,const char *key,const char *text){UmiUiValue value;UmiStatus status=umi_ui_value_set_string(&value,text);return status==UMI_STATUS_OK?umi_ui_view_model_set_property(view,key,&value):status;}
static UmiStatus set_count(UmiUiViewModel *view,const char *key,size_t count){UmiUiValue value;UmiStatus status=umi_ui_value_set_integer(&value,(int64_t)count);return status==UMI_STATUS_OK?umi_ui_view_model_set_property(view,key,&value):status;}
const char *umi_studio_visual_builder_pane_id(UmiStudioVisualBuilderPane pane){switch(pane){case UMI_STUDIO_VISUAL_BUILDER_PANE_CANVAS:return "canvas";case UMI_STUDIO_VISUAL_BUILDER_PANE_HIERARCHY:return "hierarchy";case UMI_STUDIO_VISUAL_BUILDER_PANE_PROPERTIES:return "properties";case UMI_STUDIO_VISUAL_BUILDER_PANE_BINDINGS:return "bindings";case UMI_STUDIO_VISUAL_BUILDER_PANE_INTERACTIONS:return "interactions";case UMI_STUDIO_VISUAL_BUILDER_PANE_TEMPLATES:return "templates";case UMI_STUDIO_VISUAL_BUILDER_PANE_PREVIEW:return "preview";case UMI_STUDIO_VISUAL_BUILDER_PANE_SOURCE:return "generated-source";default:return "unknown";}}
UmiStatus umi_studio_visual_builder_view_create(UmiStudioVisualBuilderCentre *centre,UmiStudioVisualBuilderPane pane,UmiUiViewModel **out_view)
{
    UmiStudioVisualBuilderSnapshot snapshot; const char *pane_id; UmiStatus status;
    if(centre==NULL||out_view==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    pane_id=umi_studio_visual_builder_pane_id(pane);
    if(pane==0||pane>UMI_STUDIO_VISUAL_BUILDER_PANE_SOURCE)return UMI_STATUS_INVALID_ARGUMENT;
    *out_view=NULL; status=umi_studio_visual_builder_centre_snapshot(centre,&snapshot);
    if(status==UMI_STATUS_OK)status=umi_ui_view_model_create(pane_id,umi_studio_visual_builder_capability_id(),UMI_UI_ROLE_PANE,out_view);
    if(status==UMI_STATUS_OK)status=set_text(*out_view,"builder.pane",pane_id);
    if(status==UMI_STATUS_OK)status=set_count(*out_view,"builder.components",snapshot.builder.document.component_count);
    if(status==UMI_STATUS_OK)status=set_count(*out_view,"builder.selection",snapshot.builder.selected_count);
    if(status==UMI_STATUS_OK)status=set_count(*out_view,"builder.bindings",snapshot.builder.binding_count);
    if(status==UMI_STATUS_OK)status=set_count(*out_view,"builder.interactions",snapshot.builder.interaction_count);
    if(status!=UMI_STATUS_OK&&*out_view!=NULL){umi_ui_view_model_destroy(*out_view);*out_view=NULL;} return status;
}
