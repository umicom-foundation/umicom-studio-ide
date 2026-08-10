/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/ui_catalogue.h
 *
 * PURPOSE:
 *   Expose an inspectable catalogue of Studio UI contracts, contributions and
 *   frontend-neutral workbench state for tests, tooling and architecture views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_UI_CATALOGUE_H
#define UMICOM_STUDIO_UI_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/studio/ui.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioUiCatalogue {
    size_t perspectives;
    size_t panes;
    size_t actions;
    size_t menu_items;
    size_t toolbar_items;
    size_t status_items;
    size_t contributions;
    size_t view_models;
    uint64_t revision;
} UmiStudioUiCatalogue;

UmiStatus umi_studio_ui_catalogue_snapshot(UmiStudioUi *ui,
                                           UmiStudioUiCatalogue *out_catalogue);
UmiStatus umi_studio_ui_catalogue_format(const UmiStudioUiCatalogue *catalogue,
                                         char *out_text,
                                         size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
