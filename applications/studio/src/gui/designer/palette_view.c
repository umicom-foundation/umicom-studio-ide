/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/designer/palette_view.c
 *
 * PURPOSE:
 *   Render the Framework component palette as a GTK4 list without hard-coding component metadata in Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is a presentation adapter.  GTK widgets are created here, while
 * authoritative application/designer state remains in the shared Framework.
 */
#include "palette_view.h"

GtkWidget *umi_studio_designer_palette_view_new(UmiStudioDeclarative *declarative)
{
    GtkWidget *scroll=gtk_scrolled_window_new();GtkWidget *list=gtk_list_box_new();UmiDesignerPalette palette;size_t i;
    if(declarative==NULL)return scroll;
    (void)umi_designer_palette_build(umi_studio_declarative_components(declarative),NULL,&palette);
    /* Each row is derived from Framework metadata so plug-in components can join the same palette later. */
    for(i=0U;i<palette.count;++i){char label[320];GtkWidget *row;g_snprintf(label,sizeof(label),"%s — %s",palette.items[i].display_name,palette.items[i].category);row=gtk_label_new(label);gtk_label_set_xalign(GTK_LABEL(row),0.0F);gtk_list_box_append(GTK_LIST_BOX(list),row);}
    umi_designer_palette_dispose(&palette);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),list);gtk_widget_set_size_request(scroll,220,-1);return scroll;
}
