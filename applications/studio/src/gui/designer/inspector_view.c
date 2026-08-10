/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/designer/inspector_view.c
 *
 * PURPOSE:
 *   Render semantic component properties as read-only rows; edits continue through designer operations rather than direct widget mutation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is a presentation adapter.  GTK widgets are created here, while
 * authoritative application/designer state remains in the shared Framework.
 */
#include "inspector_view.h"

GtkWidget *umi_studio_designer_inspector_view_new(UmiStudioDesigner *designer,const char *node_id)
{
    GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL,6);UmiDesignerInspectorSnapshot snapshot;size_t i;
    if(designer==NULL||node_id==NULL)return box;
    if(umi_designer_inspect(umi_studio_designer_document(designer),node_id,&snapshot)!=UMI_STATUS_OK)return box;
    for(i=0U;i<snapshot.attribute_count;++i){char line[640];g_snprintf(line,sizeof(line),"%s = %s",snapshot.attributes[i].name,snapshot.attributes[i].value.text);GtkWidget *label=gtk_label_new(line);gtk_label_set_xalign(GTK_LABEL(label),0.0F);gtk_box_append(GTK_BOX(box),label);}
    return box;
}
