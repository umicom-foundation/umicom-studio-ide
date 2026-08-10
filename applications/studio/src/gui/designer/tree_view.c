/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/designer/tree_view.c
 *
 * PURPOSE:
 *   Render the semantic component hierarchy as a simple GTK4 list for the reference visual designer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is a presentation adapter.  GTK widgets are created here, while
 * authoritative application/designer state remains in the shared Framework.
 */
#include "tree_view.h"

GtkWidget *umi_studio_designer_tree_view_new(UmiStudioDesigner *designer)
{
    GtkWidget *scroll=gtk_scrolled_window_new();GtkWidget *list=gtk_list_box_new();UmiDeclDocument *document;size_t i,count;
    if(designer==NULL)return scroll;document=umi_designer_document_declarative(umi_studio_designer_document(designer));count=umi_decl_document_node_count(document);
    for(i=0U;i<count;++i){UmiDeclNode node;char label[320];if(umi_decl_document_node_at(document,i,&node)!=UMI_STATUS_OK)continue;g_snprintf(label,sizeof(label),"%s  [%s]",node.node_id,node.component_type);GtkWidget *row=gtk_label_new(label);gtk_label_set_xalign(GTK_LABEL(row),0.0F);gtk_list_box_append(GTK_LIST_BOX(list),row);}
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),list);return scroll;
}
