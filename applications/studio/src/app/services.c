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

#include "umicom/studio/ai_platform.h"
#include "umicom/studio/ai_tools.h"
#include "umicom/studio/developer_platform.h"
#include "umicom/studio/declarative.h"
#include "umicom/studio/delivery_platform.h"
#include "umicom/studio/designer.h"
#include "umicom/studio/fabric.h"
#include "umicom/studio/operations.h"
#include "umicom/studio/session.h"
#include "umicom/studio/version.h"
#include "umicom/studio/web_platform.h"

struct UmiStudioServices {
    UmiDiagnosticHub diagnostic_hub;
    UmiDiagnosticStore *diagnostic_store;
    UmiSettings *settings;
    UmiTaskQueue *task_queue;
    UmiDocumentStore *documents;
    UmiSessionStore *session;
    UmiRecoveryManager *recovery;
    UmiWorkspaceGraph *workspace;
    UmiFileIndex *file_index;
    UmiWatcher *watcher;
    UmiProcessSupervisor *process_supervisor;
    UmiDataServer *data_server;
    UmiStore store;
    UmiSchemaRegistry *schemas;
    UmiDispatcher *dispatcher;
    UmiInbox *inbox;
    UmiOutbox *outbox;
    UmiDeadLetterStore *dead_letters;
    UmiTopicRegistry *topics;
    UmiMessageStore *message_store;
    UmiJournalStore journal;
    UmiMessageMetricsCounter *message_metrics;
    UmiStudioOperations *operations;
    UmiStudioAiPlatform *ai_platform;
    UmiStudioDeveloperPlatform *developer_platform;
    UmiStudioDeclarative *declarative;
    UmiStudioDesigner *designer;
    UmiStudioWebPlatform *web_platform;
    UmiStudioDeliveryPlatform *delivery_platform;
    UmiClock clock;
    int published;
};

static void studio_watch_sink(const UmiWatchEvent *event, void *user_data)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    if (services == NULL || services->file_index == NULL || event == NULL) {
        return;
    }
    if (event->kind == UMI_WATCH_OVERFLOW ||
        event->kind == UMI_WATCH_RESCAN_REQUIRED || event->directory) {
        (void)umi_file_index_rebuild(services->file_index);
    } else if (event->kind == UMI_WATCH_DELETED) {
        (void)umi_file_index_remove(services->file_index, event->path);
    } else if (event->kind == UMI_WATCH_CREATED ||
               event->kind == UMI_WATCH_MODIFIED) {
        (void)umi_file_index_update(services->file_index, event->path);
    }
}

static void destroy_partial(UmiStudioServices *services)
{
    if (services == NULL) {
        return;
    }

    if (services->watcher != NULL) {
        (void)umi_watcher_stop(services->watcher);
    }
    if (services->process_supervisor != NULL) {
        (void)umi_process_supervisor_shutdown(services->process_supervisor);
    }
    if (services->task_queue != NULL) {
        (void)umi_task_queue_shutdown(services->task_queue, 1);
    }
    umi_studio_delivery_platform_destroy(services->delivery_platform);
    services->delivery_platform = NULL;
    umi_studio_web_platform_destroy(services->web_platform);
    services->web_platform = NULL;
    umi_studio_designer_destroy(services->designer);
    services->designer = NULL;
    umi_studio_declarative_destroy(services->declarative);
    services->declarative = NULL;
    umi_studio_ai_platform_destroy(services->ai_platform);
    services->ai_platform = NULL;
    umi_studio_developer_platform_destroy(services->developer_platform);
    services->developer_platform = NULL;
    umi_studio_operations_destroy(services->operations);
    services->operations = NULL;
    umi_watcher_destroy(services->watcher);
    services->watcher = NULL;
    umi_file_index_destroy(services->file_index);
    services->file_index = NULL;
    umi_workspace_graph_destroy(services->workspace);
    services->workspace = NULL;
    free(services->message_metrics);
    services->message_metrics = NULL;
    umi_journal_store_dispose(&services->journal);
    umi_message_store_destroy(services->message_store);
    services->message_store = NULL;
    umi_topic_registry_destroy(services->topics);
    services->topics = NULL;
    umi_dead_letter_store_destroy(services->dead_letters);
    services->dead_letters = NULL;
    umi_outbox_destroy(services->outbox);
    services->outbox = NULL;
    umi_inbox_destroy(services->inbox);
    services->inbox = NULL;
    umi_dispatcher_destroy(services->dispatcher);
    services->dispatcher = NULL;
    umi_schema_registry_destroy(services->schemas);
    services->schemas = NULL;
    umi_data_server_destroy(services->data_server);
    services->data_server = NULL;
    umi_process_supervisor_destroy(services->process_supervisor);
    services->process_supervisor = NULL;
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
    char current_directory[UMI_PATH_CAPACITY];
    UmiFileIndexConfig file_index_config;
    UmiWatcherConfig watcher_config;

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
    status = umi_studio_operations_create(&services->clock,
                                          &services->operations);
    if (status != UMI_STATUS_OK) {
        destroy_partial(services);
        return status;
    }

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

    status = umi_process_supervisor_create(NULL,
                                           &services->process_supervisor);
    if (status != UMI_STATUS_OK) {
        destroy_partial(services);
        return status;
    }

    {
        const char *data_path = getenv("UMICOM_STUDIO_DATA_PATH");
        status = data_path != NULL && data_path[0] != '\0'
            ? umi_data_server_create_sqlite(data_path, &services->data_server)
            : umi_data_server_create_memory(&services->data_server);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_store_from_data_server(services->data_server,
                                            &services->store);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_schema_registry_create(&services->schemas);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_dispatcher_create(services->schemas,
                                       &services->dispatcher);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_inbox_create(4096U, &services->inbox);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_outbox_create(4096U, &services->outbox);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_dead_letter_store_create(2048U,
                                              &services->dead_letters);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_topic_registry_create(256U, &services->topics);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_message_store_create(&services->store,
                                          "studio-history",
                                          &services->message_store);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_journal_store_init(&services->journal,
                                        &services->store,
                                        "studio-journal");
    }
    if (status == UMI_STATUS_OK) {
        services->message_metrics = (UmiMessageMetricsCounter *)calloc(
            1U, umi_message_metrics_counter_size());
        status = services->message_metrics != NULL
            ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK) {
        umi_message_metrics_init(services->message_metrics);
        status = umi_studio_fabric_register_defaults(services);
    }
    if (status != UMI_STATUS_OK) {
        destroy_partial(services);
        return status;
    }

    status = umi_workspace_graph_create(&services->workspace);
    if (status != UMI_STATUS_OK ||
        umi_fs_current_directory(current_directory,
                                 sizeof(current_directory)) != UMI_STATUS_OK) {
        destroy_partial(services);
        return status != UMI_STATUS_OK ? status : UMI_STATUS_IO_ERROR;
    }
    status = umi_workspace_graph_open(services->workspace,
                                      current_directory,
                                      0);
    if (status != UMI_STATUS_OK) {
        destroy_partial(services);
        return status;
    }

    file_index_config = umi_file_index_config_default(current_directory);
    status = umi_file_index_create(&file_index_config,
                                   &services->file_index);
    if (status != UMI_STATUS_OK) {
        destroy_partial(services);
        return status;
    }

    watcher_config = umi_watcher_config_default(current_directory);
    watcher_config.sink = studio_watch_sink;
    watcher_config.sink_user_data = services;
    status = umi_watcher_create(&watcher_config, &services->watcher);
    if (status != UMI_STATUS_OK) {
        destroy_partial(services);
        return status;
    }

    status = umi_studio_developer_platform_create(
        current_directory,
        &services->clock,
        &services->developer_platform
    );
    if (status != UMI_STATUS_OK) {
        destroy_partial(services);
        return status;
    }

    status = umi_studio_declarative_create(&services->declarative);
    if (status == UMI_STATUS_OK) {
        status = umi_studio_designer_create(services->declarative,
                                            &services->designer);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_web_platform_create(&services->web_platform);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_delivery_platform_create(&services->delivery_platform);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_ai_platform_create(&services->ai_platform);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_ai_tools_register_defaults(services->ai_platform);
    }
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
    PUBLISH("umicom.studio.workspace",
            services->workspace,
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.file-index",
            services->file_index,
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.watcher",
            services->watcher,
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.process-supervisor",
            services->process_supervisor,
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.data-server",
            services->data_server,
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.message-schemas",
            services->schemas,
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.message-dispatcher",
            services->dispatcher,
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.message-inbox",
            services->inbox,
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.message-outbox",
            services->outbox,
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.dead-letters",
            services->dead_letters,
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.message-journal",
            &services->journal,
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.security",
            umi_studio_operations_security(services->operations),
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.plugins",
            umi_studio_operations_plugins(services->operations),
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.metrics",
            umi_studio_operations_metrics(services->operations),
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.traces",
            umi_studio_operations_traces(services->operations),
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.audit",
            umi_studio_operations_audit(services->operations),
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.readiness",
            umi_studio_operations_readiness(services->operations),
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.operational-events",
            umi_studio_operations_events(services->operations),
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.resilience",
            umi_studio_operations_resilience(services->operations),
            UMI_SERVICE_SINGLETON | UMI_SERVICE_THREAD_SAFE);
    PUBLISH("umicom.studio.developer-platform",
            services->developer_platform,
            UMI_SERVICE_SINGLETON);
    PUBLISH("umicom.studio.build",
            umi_studio_developer_platform_build(services->developer_platform),
            UMI_SERVICE_SINGLETON);
    PUBLISH("umicom.studio.tests",
            umi_studio_developer_platform_tests(services->developer_platform),
            UMI_SERVICE_SINGLETON);
    PUBLISH("umicom.studio.terminal",
            umi_studio_developer_platform_terminal(services->developer_platform),
            UMI_SERVICE_SINGLETON);
    PUBLISH("umicom.studio.language",
            umi_studio_developer_platform_language(services->developer_platform),
            UMI_SERVICE_SINGLETON);
    PUBLISH("umicom.studio.debugger",
            umi_studio_developer_platform_debugger(services->developer_platform),
            UMI_SERVICE_SINGLETON);
    PUBLISH("umicom.studio.source-control",
            umi_studio_developer_platform_source_control(
                services->developer_platform),
            UMI_SERVICE_SINGLETON);
    PUBLISH("umicom.studio.declarative",
            services->declarative,
            UMI_SERVICE_SINGLETON);
    PUBLISH("umicom.studio.designer",
            services->designer,
            UMI_SERVICE_SINGLETON);
    PUBLISH("umicom.studio.web-platform",
            services->web_platform,
            UMI_SERVICE_SINGLETON);
    PUBLISH("umicom.studio.delivery-platform",
            services->delivery_platform,
            UMI_SERVICE_SINGLETON);
    PUBLISH("umicom.studio.ai-platform",
            services->ai_platform,
            UMI_SERVICE_SINGLETON);
    PUBLISH("umicom.studio.ai-runtime",
            umi_studio_ai_platform_runtime(services->ai_platform),
            UMI_SERVICE_SINGLETON);
    PUBLISH("umicom.studio.helix",
            umi_studio_ai_platform_helix(services->ai_platform),
            UMI_SERVICE_SINGLETON);
    status = umi_studio_designer_bind_commands(
        services->designer,
        umi_master_controller_command_registry(master));
    if (status != UMI_STATUS_OK) return status;
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

UmiWorkspaceGraph *umi_studio_services_workspace(UmiStudioServices *services)
{
    return services != NULL ? services->workspace : NULL;
}

UmiFileIndex *umi_studio_services_file_index(UmiStudioServices *services)
{
    return services != NULL ? services->file_index : NULL;
}

UmiWatcher *umi_studio_services_watcher(UmiStudioServices *services)
{
    return services != NULL ? services->watcher : NULL;
}

UmiProcessSupervisor *umi_studio_services_process_supervisor(
    UmiStudioServices *services)
{
    return services != NULL ? services->process_supervisor : NULL;
}

UmiDataServer *umi_studio_services_data_server(UmiStudioServices *services)
{
    return services != NULL ? services->data_server : NULL;
}

UmiStore *umi_studio_services_store(UmiStudioServices *services)
{
    return services != NULL ? &services->store : NULL;
}

UmiSchemaRegistry *umi_studio_services_schema_registry(UmiStudioServices *services)
{
    return services != NULL ? services->schemas : NULL;
}

UmiDispatcher *umi_studio_services_dispatcher(UmiStudioServices *services)
{
    return services != NULL ? services->dispatcher : NULL;
}

UmiInbox *umi_studio_services_inbox(UmiStudioServices *services)
{
    return services != NULL ? services->inbox : NULL;
}

UmiOutbox *umi_studio_services_outbox(UmiStudioServices *services)
{
    return services != NULL ? services->outbox : NULL;
}

UmiDeadLetterStore *umi_studio_services_dead_letters(UmiStudioServices *services)
{
    return services != NULL ? services->dead_letters : NULL;
}

UmiTopicRegistry *umi_studio_services_topics(UmiStudioServices *services)
{
    return services != NULL ? services->topics : NULL;
}

UmiMessageStore *umi_studio_services_message_store(UmiStudioServices *services)
{
    return services != NULL ? services->message_store : NULL;
}

UmiJournalStore *umi_studio_services_journal(UmiStudioServices *services)
{
    return services != NULL ? &services->journal : NULL;
}

UmiMessageMetricsCounter *umi_studio_services_message_metrics(UmiStudioServices *services)
{
    return services != NULL ? services->message_metrics : NULL;
}

UmiStudioOperations *umi_studio_services_operations(UmiStudioServices *services)
{
    return services != NULL ? services->operations : NULL;
}

UmiStudioAiPlatform *umi_studio_services_ai_platform(
    UmiStudioServices *services)
{
    return services != NULL ? services->ai_platform : NULL;
}

UmiStudioDeveloperPlatform *umi_studio_services_developer_platform(
    UmiStudioServices *services)
{
    return services != NULL ? services->developer_platform : NULL;
}

UmiStudioBuildService *umi_studio_services_build(UmiStudioServices *services)
{
    return services != NULL && services->developer_platform != NULL
        ? umi_studio_developer_platform_build(services->developer_platform)
        : NULL;
}

UmiStudioTestService *umi_studio_services_tests(UmiStudioServices *services)
{
    return services != NULL && services->developer_platform != NULL
        ? umi_studio_developer_platform_tests(services->developer_platform)
        : NULL;
}

UmiStudioTerminalService *umi_studio_services_terminal(
    UmiStudioServices *services)
{
    return services != NULL && services->developer_platform != NULL
        ? umi_studio_developer_platform_terminal(services->developer_platform)
        : NULL;
}

UmiStudioLanguageService *umi_studio_services_language(
    UmiStudioServices *services)
{
    return services != NULL && services->developer_platform != NULL
        ? umi_studio_developer_platform_language(services->developer_platform)
        : NULL;
}

UmiStudioDebuggerService *umi_studio_services_debugger(
    UmiStudioServices *services)
{
    return services != NULL && services->developer_platform != NULL
        ? umi_studio_developer_platform_debugger(services->developer_platform)
        : NULL;
}

UmiStudioSourceControlService *umi_studio_services_source_control(
    UmiStudioServices *services)
{
    return services != NULL && services->developer_platform != NULL
        ? umi_studio_developer_platform_source_control(
              services->developer_platform)
        : NULL;
}

UmiStatus umi_studio_services_open_workspace(UmiStudioServices *services,
                                             const char *root,
                                             int trusted)
{
    UmiStatus status;

    if (services == NULL || root == NULL || root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_watcher_stop(services->watcher);
    if (status == UMI_STATUS_OK) {
        status = umi_file_index_set_root(services->file_index, root);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_watcher_set_root(services->watcher, root);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workspace_graph_open(services->workspace, root, trusted);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workspace_graph_discover(services->workspace);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_file_index_rebuild(services->file_index);
    }
    return status;
}

UmiStatus umi_studio_services_close_workspace(UmiStudioServices *services)
{
    UmiStatus status;
    if (services == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_watcher_stop(services->watcher);
    status = umi_workspace_graph_close(services->workspace);
    if (status == UMI_STATUS_OK) {
        status = umi_file_index_clear(services->file_index);
    }
    return status;
}

size_t umi_studio_services_diagnostic_sink_count(
    const UmiStudioServices *services)
{
    return services != NULL
        ? umi_diagnostic_hub_count(&services->diagnostic_hub)
        : 0U;
}

UmiStudioDeclarative *umi_studio_services_declarative(UmiStudioServices *services)
{
    return services != NULL ? services->declarative : NULL;
}

UmiStudioDesigner *umi_studio_services_designer(UmiStudioServices *services)
{
    return services != NULL ? services->designer : NULL;
}

UmiStudioWebPlatform *umi_studio_services_web_platform(
    UmiStudioServices *services)
{
    return services != NULL ? services->web_platform : NULL;
}

UmiStudioDeliveryPlatform *umi_studio_services_delivery_platform(
    UmiStudioServices *services)
{
    return services != NULL ? services->delivery_platform : NULL;
}
