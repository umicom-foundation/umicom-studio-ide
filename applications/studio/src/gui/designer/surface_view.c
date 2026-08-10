/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/designer/surface_view.c
 *
 * PURPOSE:
 *   Render a lightweight component canvas from semantic geometry while keeping document state in Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is a presentation adapter.  GTK widgets are created here, while
 * authoritative application/designer state remains in the shared Framework.
 */
#include "surface_view.h"

typedef struct SurfaceData{UmiStudioDesigner *designer;}SurfaceData;
static void draw_surface(GtkDrawingArea *area,cairo_t *cr,int width,int height,gpointer user_data)
{
    SurfaceData *data=(SurfaceData *)user_data;UmiDeclDocument *document;size_t i,count;(void)area;(void)width;(void)height;
    if(data==NULL||data->designer==NULL)return;document=umi_designer_document_declarative(umi_studio_designer_document(data->designer));count=umi_decl_document_node_count(document);
    cairo_set_source_rgb(cr,0.18,0.22,0.28);cairo_paint(cr);cairo_set_line_width(cr,1.5);
    /* Geometry comes from semantic properties, so another frontend can reproduce the same design surface. */
    for(i=0U;i<count;++i){UmiDeclNode node;UmiDesignerRect rect;if(umi_decl_document_node_at(document,i,&node)!=UMI_STATUS_OK)continue;if(umi_designer_surface_get_rect(umi_studio_designer_document(data->designer),node.node_id,&rect)!=UMI_STATUS_OK)continue;cairo_set_source_rgb(cr,0.35,0.65,0.9);cairo_rectangle(cr,(double)rect.x,(double)rect.y,(double)rect.width,(double)rect.height);cairo_stroke(cr);}
}
static void destroy_surface(gpointer data){g_free(data);}
GtkWidget *umi_studio_designer_surface_view_new(UmiStudioDesigner *designer){GtkWidget *area=gtk_drawing_area_new();SurfaceData *data=g_new0(SurfaceData,1);data->designer=designer;gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area),draw_surface,data,destroy_surface);gtk_widget_set_hexpand(area,TRUE);gtk_widget_set_vexpand(area,TRUE);return area;}
