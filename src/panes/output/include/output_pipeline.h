/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/panes/output/include/output_pipeline.h
 * PURPOSE: Glue build_runner -> filters -> rich console
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OUTPUT_PIPELINE_H
#define UMICOM_OUTPUT_PIPELINE_H

/* Include GLib types used directly in this header (gboolean, GError, etc.).   */
/* Keeping this header self-contained avoids “unknown type” warnings when      */
/* included before other GLib headers in some translation units.               */
#include <glib.h>

/* Public API dependencies: these are the collaborating modules that the       */
/* pipeline wires together. Paths are kept relative to this header’s location. */
#include "../../../build/include/build_runner.h"     /* UmiBuildRunner: process exec + async IO */
#include "output_console.h"                          /* UmiOutputConsole: rich console sink     */
#include "../../../core/include/problem_router.h"    /* UmiProblemRouter: diagnostics routing   */

/* Provide C++ compatibility without affecting pure C builds.                  */
G_BEGIN_DECLS

/*-----------------------------------------------------------------------------
 * UmiOutputPipeline
 * A tiny façade that connects a running build process (UmiBuildRunner),
 * output filters/normalization (handled inside the console pipeline),
 * and the diagnostics router (UmiProblemRouter) so warnings/errors are
 * surfaced in the Problems view while raw text streams into the console.
 *---------------------------------------------------------------------------*/
typedef struct {
  UmiBuildRunner   *runner;   /* Owned by the pipeline after start; manages the
                                 child process (spawn, stdout/stderr capture).  */
  UmiOutputConsole *console;  /* Not owned; destination widget/adapter that
                                 renders build output and applies text filters. */
  UmiProblemRouter *router;   /* Not owned; receives parsed diagnostics so they
                                 can be listed/grouped in the problems panel.   */
} UmiOutputPipeline;

/*-----------------------------------------------------------------------------
 * umi_output_pipeline_start
 *
 * PURPOSE:
 *   Spawn the build runner and connect its asynchronous stdout/stderr streams
 *   into the output console + diagnostics router.
 *
 * OWNERSHIP:
 *   Takes ownership of `p->runner` on success. Caller must later stop the
 *   pipeline (umi_output_pipeline_stop), which will tear down the process and
 *   signal disconnection from sinks.
 *
 * PARAMS:
 *   p     : Pipeline instance (fields `console` and `router` must be set;
 *           `runner` should reference the runner to be started).
 *   cwd   : Working directory for the spawned process (NULL = inherit).
 *   argv  : Null-terminated argument vector for the process (argv[0] = exe).
 *   err   : Optional GError** for detailed error info on failure.
 *
 * RETURNS:
 *   TRUE  : The process was spawned and streams are wired up.
 *   FALSE : Failed to start; `err` is set when provided. Ownership of
 *           `p->runner` remains with the caller on failure.
 *---------------------------------------------------------------------------*/
gboolean
umi_output_pipeline_start(UmiOutputPipeline *p,
                          const char *cwd,
                          char * const *argv,
                          GError **err);

/*-----------------------------------------------------------------------------
 * umi_output_pipeline_stop
 *
 * PURPOSE:
 *   Gracefully stop the running process (if any), disconnect stream handlers,
 *   and release the runner owned by the pipeline.
 *
 * NOTES:
 *   Idempotent; safe to call when not running.
 *---------------------------------------------------------------------------*/
void
umi_output_pipeline_stop(UmiOutputPipeline *p);

G_END_DECLS
#endif /* UMICOM_OUTPUT_PIPELINE_H */
/*---------------------------------------------------------------------------*/