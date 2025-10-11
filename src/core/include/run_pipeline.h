/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/run_pipeline.h
 * PURPOSE: Glue run_config + build_runner + diagnostics router
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_RUN_PIPELINE_H
#define UMICOM_RUN_PIPELINE_H

#include "run_config.h"
#include "../../build/include/build_runner.h"
#include "diagnostics_router.h"

/* Creates a runner, reads run_config, starts the process, and wires diagnostics. */
gboolean umi_run_pipeline_start(UmiOutputPane *out, UmiProblemList *plist, GError **err);

/* Stop the active pipeline (if any). */
void     umi_run_pipeline_stop(void);

#endif /* UMICOM_RUN_PIPELINE_H */
