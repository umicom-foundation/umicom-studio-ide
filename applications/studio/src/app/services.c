/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/services.c
 *
 * PURPOSE:
 *   Construct and own the shared Framework service container used by Studio.
 *   Typed settings are created first so their validated values can configure
 *   later services, including the retained diagnostic-store capacity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/services.h"

#include <stdint.h>
#include <stdlib.h>

struct UmiStudioServices {
    UmiDiagnosticHub diagnostic_hub;
    UmiDiagnosticStore *diagnostic_store;
    UmiSettings *settings;
    UmiClock clock;
};

static void destroy_partial(UmiStudioServices *services)
{
    if (services == NULL) {
        return;
    }

    umi_diagnostic_store_destroy(services->diagnostic_store);
    services->diagnostic_store = NULL;
    umi_settings_destroy(services->settings);
    services->settings = NULL;
    umi_clock_dispose(&services->clock);
    free(services);
}

UmiStatus umi_studio_services_create(
    UmiDiagnosticSink initial_sink,
    void *initial_user_data,
    UmiStudioServices **out_services)
{
    UmiDiagnosticStoreConfig store_config;
    UmiStudioServices *services;
    UmiStatus status;
    int settings_loaded = 0;
    int64_t diagnostic_capacity = 0;

    if (out_services == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_services = NULL;

    services = (UmiStudioServices *)calloc(1U, sizeof(*services));
    if (services == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    status = umi_studio_settings_create(&services->settings);
    if (status != UMI_STATUS_OK) {
        destroy_partial(services);
        return status;
    }

    status = umi_studio_settings_load_if_present(
        services->settings,
        umi_studio_settings_default_path(),
        &settings_loaded
    );
    if (status != UMI_STATUS_OK) {
        destroy_partial(services);
        return status;
    }
    (void)settings_loaded;

    status = umi_settings_get_integer(
        services->settings,
        UMI_STUDIO_SETTING_DIAGNOSTIC_CAPACITY,
        &diagnostic_capacity
    );
    if (status != UMI_STATUS_OK || diagnostic_capacity <= 0) {
        destroy_partial(services);
        return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_STATE;
    }

    umi_diagnostic_hub_init(&services->diagnostic_hub);
    services->clock = umi_clock_system();

    store_config = umi_diagnostic_store_config_default();
    store_config.capacity = (size_t)diagnostic_capacity;
    status = umi_diagnostic_store_create(&store_config,
                                         &services->diagnostic_store);
    if (status != UMI_STATUS_OK) {
        destroy_partial(services);
        return status;
    }

    status = umi_diagnostic_hub_add(&services->diagnostic_hub,
                                    umi_diagnostic_store_sink,
                                    services->diagnostic_store);
    if (status != UMI_STATUS_OK) {
        destroy_partial(services);
        return status;
    }

    if (initial_sink != NULL) {
        status = umi_diagnostic_hub_add(&services->diagnostic_hub,
                                        initial_sink,
                                        initial_user_data);
        if (status != UMI_STATUS_OK) {
            destroy_partial(services);
            return status;
        }
    }

    *out_services = services;
    return UMI_STATUS_OK;
}

void umi_studio_services_destroy(UmiStudioServices *services)
{
    destroy_partial(services);
}

UmiStatus umi_studio_services_add_diagnostic_sink(
    UmiStudioServices *services,
    UmiDiagnosticSink sink,
    void *user_data)
{
    if (services == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_diagnostic_hub_add(&services->diagnostic_hub,
                                  sink,
                                  user_data);
}

UmiStatus umi_studio_services_remove_diagnostic_sink(
    UmiStudioServices *services,
    UmiDiagnosticSink sink,
    void *user_data)
{
    if (services == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_diagnostic_hub_remove(&services->diagnostic_hub,
                                     sink,
                                     user_data);
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

UmiSettings *umi_studio_services_settings(UmiStudioServices *services)
{
    return services != NULL ? services->settings : NULL;
}

UmiDiagnosticStore *umi_studio_services_diagnostic_store(
    UmiStudioServices *services)
{
    return services != NULL ? services->diagnostic_store : NULL;
}

size_t umi_studio_services_diagnostic_sink_count(
    const UmiStudioServices *services)
{
    return services != NULL
        ? umi_diagnostic_hub_count(&services->diagnostic_hub)
        : 0U;
}
