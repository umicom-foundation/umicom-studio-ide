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
#include <string.h>

#include "umicom/studio/session.h"
#include "umicom/studio/version.h"

struct UmiStudioServices {
    UmiDiagnosticHub diagnostic_hub;
    UmiDiagnosticStore *diagnostic_store;
    UmiSettings *settings;
    UmiTaskQueue *task_queue;
    UmiDocumentStore *documents;
    UmiSessionStore *session;
    UmiRecoveryManager *recovery;
    UmiClock clock;
    int published;
};

static void destroy_partial(UmiStudioServices *services)
{
    if (services == NULL) {
        return;
    }

    if (services->task_queue != NULL) {
        (void)umi_task_queue_shutdown(services->task_queue, 1);
    }
    umi_recovery_manager_destroy(services->recovery);
    services->recovery = NULL;
    umi_session_store_destroy(services->session);
    services->session = NULL;
    umi_document_store_destroy(services->documents);
    services->documents = NULL;
    umi_task_queue_destroy(services->task_queue);
    services->task_queue = NULL;
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
    UmiTaskQueueConfig task_config;
    UmiStudioServices *services;
    UmiStatus status;
    int settings_loaded = 0;
    int session_loaded = 0;
    int64_t diagnostic_capacity = 0;
    int64_t parallel_jobs = 0;

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

    status = umi_settings_get_integer(
        services->settings,
        UMI_STUDIO_SETTING_BUILD_PARALLEL_JOBS,
        &parallel_jobs
    );
    if (status != UMI_STATUS_OK || parallel_jobs <= 0) {
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

    task_config = umi_task_queue_config_default();
    task_config.worker_count = (size_t)parallel_jobs;
    task_config.capacity = 512U;
    status = umi_task_queue_create(&task_config, &services->task_queue);
    if (status != UMI_STATUS_OK) {
        destroy_partial(services);
        return status;
    }

    status = umi_document_store_create(&services->documents);
    if (status != UMI_STATUS_OK) {
        destroy_partial(services);
        return status;
    }

    status = umi_session_store_create(&services->session);
    if (status != UMI_STATUS_OK) {
        destroy_partial(services);
        return status;
    }
    status = umi_session_store_load(services->session,
                                    umi_studio_session_default_path(),
                                    &session_loaded);
    if (status != UMI_STATUS_OK) {
        destroy_partial(services);
        return status;
    }
    (void)session_loaded;

    status = umi_recovery_manager_create(umi_studio_recovery_default_root(),
                                         &services->recovery);
    if (status != UMI_STATUS_OK) {
        destroy_partial(services);
        return status;
    }

    *out_services = services;
    return UMI_STATUS_OK;
}

void umi_studio_services_destroy(UmiStudioServices *services)
{
    destroy_partial(services);
}

static UmiStatus publish_service(UmiServiceRegistry *registry,
                                 const char *service_id,
                                 void *service,
                                 uint32_t flags)
{
    UmiServiceDescriptor descriptor;

    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.service_id = service_id;
    descriptor.service_version = umi_studio_version();
    descriptor.provider_module_id = "org.umicom.studio.services";
    descriptor.service = service;
    descriptor.flags = flags;
    return umi_service_registry_register(registry, &descriptor);
}

UmiStatus umi_studio_services_publish(
    UmiStudioServices *services,
    UmiMasterController *master)
{
    UmiServiceRegistry *registry;
    UmiHealthRegistry *health;
    UmiStatus status;

    if (services == NULL || master == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (services->published) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    registry = umi_master_controller_services(master);
    health = umi_master_controller_health(master);
    if (registry == NULL || health == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

#define PUBLISH(id, service, flags)                                         \
    do {                                                                    \
        status = publish_service(registry, (id), (service), (flags));       \
        if (status != UMI_STATUS_OK) return status;                         \
    } while (0)

    PUBLISH("umicom.studio.settings",
            services->settings,
            UMI_SERVICE_SINGLETON);
    PUBLISH("umicom.studio.diagnostics.store",
            services->diagnostic_store,
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.tasks",
            services->task_queue,
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.documents",
            services->documents,
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.session",
            services->session,
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.recovery",
            services->recovery,
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.clock",
            &services->clock,
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
#undef PUBLISH

    status = umi_health_registry_update(health,
                                        "org.umicom.studio.services",
                                        UMI_HEALTH_READY,
                                        "Studio shared services are ready",
                                        services->clock.wall_nanoseconds(
                                            &services->clock));
    if (status != UMI_STATUS_OK) {
        return status;
    }

    services->published = 1;
    return UMI_STATUS_OK;
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

UmiTaskQueue *umi_studio_services_task_queue(UmiStudioServices *services)
{
    return services != NULL ? services->task_queue : NULL;
}

UmiDocumentStore *umi_studio_services_documents(UmiStudioServices *services)
{
    return services != NULL ? services->documents : NULL;
}

UmiSessionStore *umi_studio_services_session(UmiStudioServices *services)
{
    return services != NULL ? services->session : NULL;
}

UmiRecoveryManager *umi_studio_services_recovery(UmiStudioServices *services)
{
    return services != NULL ? services->recovery : NULL;
}

size_t umi_studio_services_diagnostic_sink_count(
    const UmiStudioServices *services)
{
    return services != NULL
        ? umi_diagnostic_hub_count(&services->diagnostic_hub)
        : 0U;
}
