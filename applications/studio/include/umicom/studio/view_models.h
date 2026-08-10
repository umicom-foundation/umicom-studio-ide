/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/view_models.h
 *
 * PURPOSE:
 *   Define the Studio-owned toolkit-neutral view-model set that projects
 *   Framework workspace, document, diagnostic, search and task state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_VIEW_MODELS_H
#define UMICOM_STUDIO_VIEW_MODELS_H

#include "umicom/umicom.h"
#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioViewModels UmiStudioViewModels;

typedef struct UmiStudioViewModelSnapshot {
    size_t view_count;
    size_t workspace_files;
    size_t open_documents;
    size_t dirty_documents;
    size_t retained_diagnostics;
    size_t queued_tasks;
    size_t running_tasks;
    uint64_t revision;
} UmiStudioViewModelSnapshot;

UmiStatus umi_studio_view_models_create(UmiStudioServices *services,
                                        UmiStudioViewModels **out_models);
void umi_studio_view_models_destroy(UmiStudioViewModels *models);
UmiStatus umi_studio_view_models_refresh(UmiStudioViewModels *models);
UmiStatus umi_studio_view_models_snapshot(const UmiStudioViewModels *models,
                                          UmiStudioViewModelSnapshot *out_snapshot);
UmiUiViewModel *umi_studio_view_models_find(UmiStudioViewModels *models,
                                           const char *view_id);
size_t umi_studio_view_models_count(const UmiStudioViewModels *models);

#ifdef __cplusplus
}
#endif

#endif
