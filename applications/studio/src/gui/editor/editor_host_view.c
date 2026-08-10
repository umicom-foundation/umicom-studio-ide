/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/editor/editor_host_view.c
 *
 * PURPOSE:
 *   Render open-document summaries without making a GTK text buffer the
 *   authoritative document state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "editor_host_view.h"

#include <stdio.h>

GtkWidget *umi_studio_editor_host_view_new(UmiStudioUi *ui)
{
    GtkWidget *box;
    GtkWidget *title;
    GtkWidget *summary;
    size_t open_documents;
    char text[256];
    if (ui == NULL) return NULL;
    open_documents = umi_document_store_count(
        umi_studio_services_documents(umi_studio_ui_services(ui)));
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    title = gtk_label_new("Editor");
    gtk_widget_add_css_class(title, "title-2");
    (void)snprintf(text, sizeof(text), "Open documents: %zu\n"
                   "Framework document state is authoritative.", open_documents);
    summary = gtk_label_new(text);
    gtk_label_set_wrap(GTK_LABEL(summary), TRUE);
    gtk_box_append(GTK_BOX(box), title);
    gtk_box_append(GTK_BOX(box), summary);
    return box;
}
