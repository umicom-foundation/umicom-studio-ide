/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/core/include/run_pipeline.h
 *
 * PURPOSE:
 *   Tiny orchestration API to: (a) load run config, (b) spawn the target
 *   process (via build runner), and (c) pipe output through diagnostics.
 *
 * DESIGN:
 *   - Public header depends only on public module headers by name.
 *   - Implementation keeps a small context (runner + router) while running.
 *   - No deep/relative include paths.
 *
 * API:
 *   gboolean umi_run_pipeline_start(UmiOutputPane *out,
 *                                   UmiProblemList *plist,
 *                                   GError **err);
 *   void     umi_run_pipeline_stop(void);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUN_PIPELINE_H
#define UMICOM_RUN_PIPELINE_H

#include <glib.h>                /* GError, gboolean                          */

/* Forward declarations for sinks */
typedef struct _UmiOutputPane  UmiOutputPane;
typedef struct _UmiProblemList UmiProblemList;

/* Start the run (spawn process and wire output). Returns TRUE on success.    */
gboolean umi_run_pipeline_start(UmiOutputPane  *out,
                                UmiProblemList *plist,
                                GError        **err);

/* Politely stop the running process (if any).                                */
void     umi_run_pipeline_stop(void);

#endif /* UMICOM_RUN_PIPELINE_H */
