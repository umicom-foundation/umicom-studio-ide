/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/ui_catalogue.c
 *
 * PURPOSE:
 *   Create an inspectable summary of Studio workbench contracts and UI
 *   contributions for native tooling and architecture inspection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/ui_catalogue.h"

#include <stdio.h>

#include "umicom/studio/view_models.h"

UmiStatus umi_studio_ui_catalogue_snapshot(UmiStudioUi *ui,
                                           UmiStudioUiCatalogue *out_catalogue)
{
    UmiUiWorkbench *workbench;
    UmiUiWorkbenchSnapshot snapshot;
    UmiStudioViewModelSnapshot views;
    UmiStatus status;
    if (ui == NULL || out_catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    workbench = umi_studio_ui_workbench(ui);
    status = umi_ui_workbench_snapshot(workbench, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    status = umi_studio_view_models_snapshot(umi_studio_ui_view_models(ui), &views);
    if (status != UMI_STATUS_OK) return status;
    out_catalogue->perspectives = snapshot.perspective_count;
    out_catalogue->panes = snapshot.pane_count;
    out_catalogue->actions = snapshot.action_count;
    out_catalogue->menu_items = umi_ui_menu_model_count(umi_ui_workbench_menus(workbench));
    out_catalogue->toolbar_items = umi_ui_toolbar_model_count(umi_ui_workbench_toolbars(workbench));
    out_catalogue->status_items = umi_ui_status_model_count(umi_ui_workbench_status(workbench));
    out_catalogue->contributions = umi_ui_contribution_model_count(umi_ui_workbench_contributions(workbench));
    out_catalogue->view_models = views.view_count;
    out_catalogue->revision = snapshot.revision + views.revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_ui_catalogue_format(const UmiStudioUiCatalogue *catalogue,
                                         char *out_text,
                                         size_t capacity)
{
    int written;
    if (catalogue == NULL || out_text == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(out_text, capacity,
                       "Studio UI Catalogue\n"
                       "Perspectives: %zu\nPanes: %zu\nActions: %zu\n"
                       "Menu items: %zu\nToolbar items: %zu\nStatus items: %zu\n"
                       "Contributions: %zu\nView models: %zu\nRevision: %llu\n",
                       catalogue->perspectives, catalogue->panes, catalogue->actions,
                       catalogue->menu_items, catalogue->toolbar_items, catalogue->status_items,
                       catalogue->contributions, catalogue->view_models,
                       (unsigned long long)catalogue->revision);
    if (written < 0 || (size_t)written >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}
