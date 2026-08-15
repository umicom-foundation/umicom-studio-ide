/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/services.h
 *
 * PURPOSE:
 *   Own the Umicom Framework services shared by Studio frontends and Slave
 *   Controllers.  The container creates one instance of each authoritative
 *   settings, diagnostics, task, document, session, recovery, workspace, file
 *   index, watcher, process-supervision, Data Server, Integration Fabric and
 *   clock service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_SERVICES_H
#define UMICOM_STUDIO_SERVICES_H

#include <stddef.h>

#include "umicom/umicom.h"
#include "umicom/studio/ai_platform.h"
#include "umicom/studio/compatibility_platform.h"
#include "umicom/studio/developer_platform.h"
#include "umicom/studio/declarative.h"
#include "umicom/studio/delivery_platform.h"
#include "umicom/studio/designer.h"
#include "umicom/studio/extension_platform.h"
#include "umicom/studio/product_centre.h"
#include "umicom/studio/settings.h"
#include "umicom/studio/trading.h"
#include "umicom/studio/web_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioServices UmiStudioServices;
typedef struct UmiStudioOperations UmiStudioOperations;

UmiStatus umi_studio_services_create(
    UmiDiagnosticSink initial_sink,
    void *initial_user_data,
    UmiStudioServices **out_services
);

void umi_studio_services_destroy(UmiStudioServices *services);

UmiStatus umi_studio_services_publish(
    UmiStudioServices *services,
    UmiMasterController *master
);

UmiStatus umi_studio_services_add_diagnostic_sink(
    UmiStudioServices *services,
    UmiDiagnosticSink sink,
    void *user_data
);

UmiStatus umi_studio_services_remove_diagnostic_sink(
    UmiStudioServices *services,
    UmiDiagnosticSink sink,
    void *user_data
);

UmiDiagnosticSink umi_studio_services_diagnostic_sink(void);
void *umi_studio_services_diagnostic_user_data(UmiStudioServices *services);
UmiClock *umi_studio_services_clock(UmiStudioServices *services);
UmiSettings *umi_studio_services_settings(UmiStudioServices *services);
UmiDiagnosticStore *umi_studio_services_diagnostic_store(
    UmiStudioServices *services
);
UmiDiagnosticPipeline *umi_studio_services_diagnostic_pipeline(
    UmiStudioServices *services
);
UmiTaskQueue *umi_studio_services_task_queue(UmiStudioServices *services);
UmiDocumentStore *umi_studio_services_documents(UmiStudioServices *services);
UmiSessionStore *umi_studio_services_session(UmiStudioServices *services);
UmiRecoveryManager *umi_studio_services_recovery(UmiStudioServices *services);
UmiWorkspaceGraph *umi_studio_services_workspace(UmiStudioServices *services);
UmiFileIndex *umi_studio_services_file_index(UmiStudioServices *services);
UmiWatcher *umi_studio_services_watcher(UmiStudioServices *services);
UmiProcessSupervisor *umi_studio_services_process_supervisor(
    UmiStudioServices *services
);
UmiDataServer *umi_studio_services_data_server(UmiStudioServices *services);
UmiStore *umi_studio_services_store(UmiStudioServices *services);
UmiSchemaRegistry *umi_studio_services_schema_registry(UmiStudioServices *services);
UmiDispatcher *umi_studio_services_dispatcher(UmiStudioServices *services);
UmiInbox *umi_studio_services_inbox(UmiStudioServices *services);
UmiOutbox *umi_studio_services_outbox(UmiStudioServices *services);
UmiDeadLetterStore *umi_studio_services_dead_letters(UmiStudioServices *services);
UmiTopicRegistry *umi_studio_services_topics(UmiStudioServices *services);
UmiMessageStore *umi_studio_services_message_store(UmiStudioServices *services);
UmiJournalStore *umi_studio_services_journal(UmiStudioServices *services);
UmiMessageMetricsCounter *umi_studio_services_message_metrics(UmiStudioServices *services);
UmiStudioOperations *umi_studio_services_operations(UmiStudioServices *services);
UmiStudioAiPlatform *umi_studio_services_ai_platform(
    UmiStudioServices *services
);
UmiKnowledgeService *umi_studio_services_knowledge(
    UmiStudioServices *services
);
UmiStudioDeveloperPlatform *umi_studio_services_developer_platform(
    UmiStudioServices *services
);
UmiStudioDeclarative *umi_studio_services_declarative(UmiStudioServices *services);
UmiStudioDesigner *umi_studio_services_designer(UmiStudioServices *services);
UmiStudioWebPlatform *umi_studio_services_web_platform(UmiStudioServices *services);
UmiStudioDeliveryPlatform *umi_studio_services_delivery_platform(
    UmiStudioServices *services);
UmiStudioCompatibilityPlatform *umi_studio_services_compatibility_platform(
    UmiStudioServices *services
);
UmiStudioExtensionPlatform *umi_studio_services_extension_platform(
    UmiStudioServices *services
);
UmiStudioProductCentre *umi_studio_services_product_centre(
    UmiStudioServices *services
);
UmiStudioBuildService *umi_studio_services_build(UmiStudioServices *services);
UmiStudioTestService *umi_studio_services_tests(UmiStudioServices *services);
UmiStudioTerminalService *umi_studio_services_terminal(
    UmiStudioServices *services
);
UmiTerminalController *umi_studio_services_terminal_controller(
    UmiStudioServices *services
);
UmiStudioLanguageService *umi_studio_services_language(
    UmiStudioServices *services
);
UmiStudioDebuggerService *umi_studio_services_debugger(
    UmiStudioServices *services
);
UmiStudioSourceControlService *umi_studio_services_source_control(
    UmiStudioServices *services
);
UmiStudioTradingService *umi_studio_services_trading(
    UmiStudioServices *services
);
UmiStatus umi_studio_services_open_workspace(UmiStudioServices *services,
                                             const char *root,
                                             int trusted);
UmiStatus umi_studio_services_close_workspace(UmiStudioServices *services);

size_t umi_studio_services_diagnostic_sink_count(
    const UmiStudioServices *services
);

#ifdef __cplusplus
}
#endif

#endif
