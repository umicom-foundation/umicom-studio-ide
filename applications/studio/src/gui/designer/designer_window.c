/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/designer/designer_window.c
 *
 * PURPOSE:
 *   Compose palette, component tree, design surface, inspector and preview into a reference GTK4 designer window.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is a presentation adapter.  GTK widgets are created here, while
 * authoritative application/designer state remains in the shared Framework.
 */
#include "designer_window.h"

#include "palette_view.h"
#include "tree_view.h"
#include "inspector_view.h"
#include "surface_view.h"
#include "preview_view.h"
GtkWidget *umi_studio_designer_window_new(GtkApplication *application,UmiStudioDeclarative *declarative,UmiStudioDesigner *designer)
{
    GtkWidget *window=gtk_application_window_new(application);GtkWidget *root=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);GtkWidget *paned=gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);GtkWidget *right=gtk_paned_new(GTK_ORIENTATION_VERTICAL);GtkWidget *left=gtk_box_new(GTK_ORIENTATION_VERTICAL,4);
    gtk_window_set_title(GTK_WINDOW(window),"Umicom Studio — Visual Designer");gtk_window_set_default_size(GTK_WINDOW(window),1200,760);
    gtk_box_append(GTK_BOX(left),umi_studio_designer_palette_view_new(declarative));gtk_box_append(GTK_BOX(left),umi_studio_designer_tree_view_new(designer));
    gtk_paned_set_start_child(GTK_PANED(paned),left);gtk_paned_set_end_child(GTK_PANED(paned),right);gtk_paned_set_start_child(GTK_PANED(right),umi_studio_designer_surface_view_new(designer));
    /* The bottom area deliberately shows both semantic properties and deterministic preview evidence. */
    GtkWidget *bottom=gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);gtk_paned_set_start_child(GTK_PANED(bottom),umi_studio_designer_inspector_view_new(designer,"root"));gtk_paned_set_end_child(GTK_PANED(bottom),umi_studio_designer_preview_view_new(designer));gtk_paned_set_end_child(GTK_PANED(right),bottom);
    gtk_box_append(GTK_BOX(root),paned);gtk_window_set_child(GTK_WINDOW(window),root);return window;
}
