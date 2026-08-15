/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/application_centre.h
 *
 * PURPOSE:
 *   Define Studio application-centre composition for Umicom products and runtimes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio composes reusable Framework capabilities here. Generic models and
 * engines stay in Framework so the same mechanisms can serve future products.
 */
#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_APPLICATION_CENTRE_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_APPLICATION_CENTRE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/umicom.h"
#include "umicom/studio/services.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioApplicationCentreSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char area_id[128];
    char view_type[128];
    char title[256];
    char summary[512];
    uint64_t revision;
    size_t item_count;
    size_t available_count;
    size_t foundation_count;
    size_t roadmap_count;
    size_t reusable_component_count;
    size_t gtk4_widget_count;
    size_t architecture_error_count;
    int architecture_passed;
    int available;
} UmiStudioApplicationCentreSnapshot;
UmiStatus umi_studio_application_centre_snapshot(UmiStudioServices *services, UmiStudioApplicationCentreSnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif
#endif
