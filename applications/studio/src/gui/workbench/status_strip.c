/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/workbench/status_strip.c
 *
 * PURPOSE:
 *   Render visible Framework status items as a GTK4 horizontal status strip.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "status_strip.h"

GtkWidget *umi_studio_status_strip_new(UmiStudioUi *ui)
{
    GtkWidget *box;
    UmiUiStatusSnapshot item;
    UmiUiStatusModel *model;
    size_t index;
    if (ui == NULL) return NULL;
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    model = umi_ui_workbench_status(umi_studio_ui_workbench(ui));
    for (index = 0U; index < umi_ui_status_model_count(model); ++index) {
        GtkWidget *label;
        if (umi_ui_status_model_at(model, index, &item) != UMI_STATUS_OK || !item.visible) continue;
        label = gtk_label_new(item.text);
        gtk_widget_set_tooltip_text(label, item.tooltip);
        gtk_box_append(GTK_BOX(box), label);
    }
    return box;
}
