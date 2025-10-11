/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/output_pipeline.h
 * PURPOSE: Glue build_runner -> filters -> rich console
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OUTPUT_PIPELINE_H
#define UMICOM_OUTPUT_PIPELINE_H

#include "../../../build/include/build_runner.h"
#include "output_console.h"
#include "../../../core/include/problem_router.h"

typedef struct {
  UmiBuildRunner *runner;
  UmiOutputConsole *console;
  UmiProblemRouter *router;
} UmiOutputPipeline;

/* Start a pipeline; takes ownership of runner; returns TRUE if spawn ok. */
gboolean umi_output_pipeline_start(UmiOutputPipeline *p,
                                   const char *cwd,
                                   char * const *argv,
                                   GError **err);

/* Stop pipeline (if any). */
void umi_output_pipeline_stop(UmiOutputPipeline *p);

#endif /* UMICOM_OUTPUT_PIPELINE_H */
