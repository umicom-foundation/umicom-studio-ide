/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/processes.h
 *
 * PURPOSE:
 *   Submit, cancel, wait for and inspect supervised Studio child processes.
 *   Execution is restricted to trusted workspaces and defaults to the active
 *   workspace root when no working directory is supplied.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_PROCESSES_H
#define UMICOM_STUDIO_PROCESSES_H

#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_process_submit(UmiStudioServices *services,
                                    const char *label,
                                    const UmiProcessRequest *request,
                                    UmiProcessJobId *out_job_id);
UmiStatus umi_studio_process_cancel(UmiStudioServices *services,
                                    UmiProcessJobId job_id);
UmiStatus umi_studio_process_wait(UmiStudioServices *services,
                                  UmiProcessJobId job_id,
                                  uint32_t timeout_ms);
UmiStatus umi_studio_process_snapshot(
    UmiStudioServices *services,
    UmiProcessJobId job_id,
    UmiProcessJobSnapshot *out_snapshot
);
UmiProcessSupervisorStats umi_studio_process_stats(
    UmiStudioServices *services
);

#ifdef __cplusplus
}
#endif

#endif
