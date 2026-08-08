/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/services.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/services.h"

#include <stdlib.h>

struct UmiStudioServices {
    UmiDiagnosticHub diagnostic_hub;
    UmiClock clock;
};

UmiStatus umi_studio_services_create(UmiDiagnosticSink initial_sink,
                                     void *initial_user_data,
                                     UmiStudioServices **out_services)
{
    UmiStudioServices *services;
    UmiStatus status;
    if (out_services == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_services = NULL;
    services = (UmiStudioServices *)calloc(1U, sizeof(*services));
    if (services == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    umi_diagnostic_hub_init(&services->diagnostic_hub);
    services->clock = umi_clock_system();
    if (initial_sink != NULL) {
        status = umi_diagnostic_hub_add(&services->diagnostic_hub,
                                        initial_sink,
                                        initial_user_data);
        if (status != UMI_STATUS_OK) {
            umi_clock_dispose(&services->clock);
            free(services);
            return status;
        }
    }
    *out_services = services;
    return UMI_STATUS_OK;
}

void umi_studio_services_destroy(UmiStudioServices *services)
{
    if (services == NULL) {
        return;
    }
    umi_clock_dispose(&services->clock);
    free(services);
}

UmiStatus umi_studio_services_add_diagnostic_sink(UmiStudioServices *services,
                                                  UmiDiagnosticSink sink,
                                                  void *user_data)
{
    if (services == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_diagnostic_hub_add(&services->diagnostic_hub, sink, user_data);
}

UmiDiagnosticSink umi_studio_services_diagnostic_sink(void)
{
    return umi_diagnostic_hub_emit;
}

void *umi_studio_services_diagnostic_user_data(UmiStudioServices *services)
{
    return services != NULL ? (void *)&services->diagnostic_hub : NULL;
}

UmiClock *umi_studio_services_clock(UmiStudioServices *services)
{
    return services != NULL ? &services->clock : NULL;
}

size_t umi_studio_services_diagnostic_sink_count(const UmiStudioServices *services)
{
    return services != NULL ? umi_diagnostic_hub_count(&services->diagnostic_hub) : 0U;
}
