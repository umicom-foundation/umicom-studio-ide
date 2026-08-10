/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/view_models.c
 *
 * PURPOSE:
 *   Project authoritative Framework service state into bounded Studio view models
 *   without storing GTK4 widgets in application state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/view_models.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiStudioViewModels {
    UmiStudioServices *services;
    UmiUiViewModel *views[5];
    uint64_t revision;
};

static const char *const VIEW_IDS[] = {
    "studio.view.project-explorer",
    "studio.view.editor",
    "studio.view.output",
    "studio.view.problems",
    "studio.view.search"
};

static const char *const VIEW_TYPES[] = {
    "studio.project-explorer",
    "studio.editor-host",
    "studio.output",
    "studio.problems",
    "studio.search"
};

static const UmiUiRole VIEW_ROLES[] = {
    UMI_UI_ROLE_PANE,
    UMI_UI_ROLE_EDITOR,
    UMI_UI_ROLE_PANE,
    UMI_UI_ROLE_PANE,
    UMI_UI_ROLE_PANE
};

static UmiStatus set_integer(UmiUiViewModel *view, const char *key, int64_t number)
{
    UmiUiValue value;
    umi_ui_value_clear(&value);
    (void)umi_ui_value_set_integer(&value, number);
    return umi_ui_view_model_set_property(view, key, &value);
}

static UmiStatus set_string(UmiUiViewModel *view, const char *key, const char *text)
{
    UmiUiValue value;
    umi_ui_value_clear(&value);
    (void)umi_ui_value_set_string(&value, text != NULL ? text : "");
    return umi_ui_view_model_set_property(view, key, &value);
}

UmiStatus umi_studio_view_models_create(UmiStudioServices *services,
                                        UmiStudioViewModels **out_models)
{
    UmiStudioViewModels *models;
    size_t index;
    UmiStatus status;

    if (services == NULL || out_models == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_models = NULL;
    models = (UmiStudioViewModels *)calloc(1U, sizeof(*models));
    if (models == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    models->services = services;
    models->revision = 1U;
    for (index = 0U; index < 5U; ++index) {
        status = umi_ui_view_model_create(VIEW_IDS[index], VIEW_TYPES[index],
                                          VIEW_ROLES[index], &models->views[index]);
        if (status != UMI_STATUS_OK) {
            umi_studio_view_models_destroy(models);
            return status;
        }
    }
    status = set_string(models->views[0], "title", "Project Explorer");
    if (status == UMI_STATUS_OK) status = set_string(models->views[1], "title", "Editor");
    if (status == UMI_STATUS_OK) status = set_string(models->views[2], "title", "Output");
    if (status == UMI_STATUS_OK) status = set_string(models->views[3], "title", "Problems");
    if (status == UMI_STATUS_OK) status = set_string(models->views[4], "title", "Search");
    if (status != UMI_STATUS_OK) {
        umi_studio_view_models_destroy(models);
        return status;
    }
    status = umi_studio_view_models_refresh(models);
    if (status != UMI_STATUS_OK) {
        umi_studio_view_models_destroy(models);
        return status;
    }
    *out_models = models;
    return UMI_STATUS_OK;
}

void umi_studio_view_models_destroy(UmiStudioViewModels *models)
{
    size_t index;
    if (models == NULL) return;
    for (index = 0U; index < 5U; ++index) {
        umi_ui_view_model_destroy(models->views[index]);
    }
    free(models);
}

UmiStatus umi_studio_view_models_refresh(UmiStudioViewModels *models)
{
    UmiWorkspaceGraphSnapshot workspace = {0};
    UmiFileIndexStats files;
    UmiDiagnosticStoreSummary diagnostics = {0};
    UmiTaskQueueStats tasks;
    UmiDocumentSnapshot document;
    size_t document_count;
    size_t dirty = 0U;
    size_t index;
    UmiStatus status;

    if (models == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    files = umi_file_index_stats(umi_studio_services_file_index(models->services));
    tasks = umi_task_queue_stats(umi_studio_services_task_queue(models->services));
    document_count = umi_document_store_count(umi_studio_services_documents(models->services));
    (void)umi_workspace_graph_snapshot(umi_studio_services_workspace(models->services), &workspace);
    (void)umi_diagnostic_store_summary(umi_studio_services_diagnostic_store(models->services), &diagnostics);
    for (index = 0U; index < document_count; ++index) {
        if (umi_document_store_at(umi_studio_services_documents(models->services), index,
                                  &document) == UMI_STATUS_OK && document.dirty) {
            dirty += 1U;
        }
    }

    status = set_string(models->views[0], "workspace.root", workspace.root);
    if (status == UMI_STATUS_OK) status = set_integer(models->views[0], "workspace.files", (int64_t)files.files);
    if (status == UMI_STATUS_OK) status = set_integer(models->views[0], "workspace.projects", (int64_t)workspace.project_count);
    if (status == UMI_STATUS_OK) status = set_integer(models->views[1], "documents.open", (int64_t)document_count);
    if (status == UMI_STATUS_OK) status = set_integer(models->views[1], "documents.dirty", (int64_t)dirty);
    if (status == UMI_STATUS_OK) status = set_integer(models->views[2], "tasks.queued", (int64_t)tasks.queued);
    if (status == UMI_STATUS_OK) status = set_integer(models->views[2], "tasks.running", (int64_t)tasks.running);
    if (status == UMI_STATUS_OK) status = set_integer(models->views[3], "diagnostics.retained", (int64_t)diagnostics.retained_count);
    if (status == UMI_STATUS_OK) status = set_integer(models->views[4], "index.files", (int64_t)files.files);
    if (status == UMI_STATUS_OK) models->revision += 1U;
    return status;
}

UmiStatus umi_studio_view_models_snapshot(const UmiStudioViewModels *models,
                                          UmiStudioViewModelSnapshot *out_snapshot)
{
    UmiFileIndexStats files;
    UmiDiagnosticStoreSummary diagnostics = {0};
    UmiTaskQueueStats tasks;
    UmiDocumentSnapshot document;
    size_t count;
    size_t dirty = 0U;
    size_t index;
    if (models == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    files = umi_file_index_stats(umi_studio_services_file_index(models->services));
    tasks = umi_task_queue_stats(umi_studio_services_task_queue(models->services));
    count = umi_document_store_count(umi_studio_services_documents(models->services));
    (void)umi_diagnostic_store_summary(umi_studio_services_diagnostic_store(models->services), &diagnostics);
    for (index = 0U; index < count; ++index) {
        if (umi_document_store_at(umi_studio_services_documents(models->services), index,
                                  &document) == UMI_STATUS_OK && document.dirty) dirty += 1U;
    }
    out_snapshot->view_count = 5U;
    out_snapshot->workspace_files = files.files;
    out_snapshot->open_documents = count;
    out_snapshot->dirty_documents = dirty;
    out_snapshot->retained_diagnostics = diagnostics.retained_count;
    out_snapshot->queued_tasks = tasks.queued;
    out_snapshot->running_tasks = tasks.running;
    out_snapshot->revision = models->revision;
    return UMI_STATUS_OK;
}

UmiUiViewModel *umi_studio_view_models_find(UmiStudioViewModels *models,
                                           const char *view_id)
{
    size_t index;
    if (models == NULL || view_id == NULL) return NULL;
    for (index = 0U; index < 5U; ++index) {
        if (strcmp(VIEW_IDS[index], view_id) == 0) return models->views[index];
    }
    return NULL;
}

size_t umi_studio_view_models_count(const UmiStudioViewModels *models)
{
    return models != NULL ? 5U : 0U;
}
