/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/sdk_centre.c
 *
 * PURPOSE:
 *   Present installed Framework SDK exports to Studio project consumers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/sdk_centre.h"
#include <string.h>

UmiStatus umi_studio_sdk_centre_refresh(UmiStudioSdkCentre *centre)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    centre->consumer_cmake[0] = '\0';
    status = umi_sdk_export_plan_validate(&centre->export_plan);
    if (status != UMI_STATUS_OK) {
        centre->ready = 0;
        return status;
    }
    status = umi_sdk_consumer_package_render_cmake(
        &centre->export_plan.package, &centre->export_plan.catalogue,
        centre->consumer_cmake, sizeof(centre->consumer_cmake));
    centre->ready = status == UMI_STATUS_OK;
    return status;
}

UmiStatus umi_studio_sdk_centre_init(UmiStudioSdkCentre *centre,
                                         const char *install_prefix)
{
    static const struct {
        const char *component;
        const char *target;
    } exports[] = {
        {"base", "Umicom::base"},
        {"runtime", "Umicom::runtime"},
        {"ui", "Umicom::ui"},
        {"compiler", "Umicom::compiler"},
        {"delivery", "Umicom::delivery"},
        {"sdk", "Umicom::sdk"},
        {"product", "Umicom::product"},
        {"distribution", "Umicom::distribution"}
    };
    UmiStatus status;
    size_t index;
    if (centre == NULL || install_prefix == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(centre, 0, sizeof(*centre));
    status = umi_sdk_export_plan_init(&centre->export_plan, install_prefix,
                                          "0.9");
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < sizeof(exports) / sizeof(exports[0]); ++index) {
        status = umi_sdk_export_plan_add_target(
            &centre->export_plan, exports[index].component,
            exports[index].target, 1);
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_studio_sdk_centre_refresh(centre);
}

const char *umi_studio_sdk_centre_consumer_cmake(
    const UmiStudioSdkCentre *centre)
{
    return centre != NULL && centre->ready ? centre->consumer_cmake : NULL;
}
