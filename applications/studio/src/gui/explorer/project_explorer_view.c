/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/explorer/project_explorer_view.c
 *
 * PURPOSE:
 *   Render workspace identity, trust and indexed-file counts from Framework
 *   services without performing filesystem operations in the widget.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "project_explorer_view.h"

#include <stdio.h>

GtkWidget *umi_studio_project_explorer_view_new(UmiStudioUi *ui)
{
    GtkWidget *box;
    GtkWidget *title;
    GtkWidget *summary;
    UmiWorkspaceGraphSnapshot workspace = {0};
    UmiFileIndexStats files;
    char text[768];
    UmiStudioServices *services;
    if (ui == NULL) return NULL;
    services = umi_studio_ui_services(ui);
    files = umi_file_index_stats(umi_studio_services_file_index(services));
    (void)umi_workspace_graph_snapshot(umi_studio_services_workspace(services), &workspace);
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    title = gtk_label_new("Project Explorer");
    gtk_widget_add_css_class(title, "title-3");
    (void)snprintf(text, sizeof(text), "Root: %s\nProjects: %zu\nFiles: %zu\nTrusted: %s",
                   workspace.root[0] != '\0' ? workspace.root : "No workspace",
                   workspace.project_count, files.files, workspace.trusted ? "yes" : "no");
    summary = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(summary), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(summary), TRUE);
    gtk_box_append(GTK_BOX(box), title);
    gtk_box_append(GTK_BOX(box), summary);
    return box;
}
