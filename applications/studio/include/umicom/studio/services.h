/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/services.h
 *
 * PURPOSE:
 *   Own the Umicom Framework services shared by Studio frontends and Slave
 *   Controllers.  The container creates one instance of each authoritative
 *   settings, diagnostics, task, document, session, recovery, workspace, file
 *   index, watcher, process-supervision and clock service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_SERVICES_H
#define UMICOM_STUDIO_SERVICES_H

#include <stddef.h>

#include "umicom/umicom.h"
#include "umicom/studio/settings.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioServices UmiStudioServices;

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
