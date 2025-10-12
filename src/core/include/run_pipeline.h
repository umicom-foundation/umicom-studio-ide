/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/core/include/run_pipeline.h
 *
 * PROJECT:
 *   Umicom Studio IDE — Core “run pipeline” public interface.
 *
 * PURPOSE:
 *   Provide a tiny, stable API for the core to (a) read run configuration,
 *   (b) spawn the target process via the build runner, and (c) wire stdout/
 *   stderr into the diagnostics router and the Output pane.
 *
 * DESIGN / COUPLING NOTES:
 *   - Self-contained includes: we include ONLY the public headers that define
 *     the types we reference. No relative cross-folder paths (no "../../...").
 *   - Loosely coupled: this header exposes just two functions; callers do not
 *     need to know anything about GLib internals or how the process is wired.
 *   - Ownership: the pipeline manages its own runner instance and router state.
 *
 * EXTERNAL REQUIREMENTS:
 *   - GLib/GIO available (transitively via included headers).
 *   - A valid UmiOutputPane* and UmiProblemList* supplied by the GUI/problem UI.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_RUN_PIPELINE_H
#define UMICOM_RUN_PIPELINE_H

/* Public dependencies (module-level, not via relative paths). */
#include <glib.h>                 /* GError, gboolean                                 */
#include "run_config.h"           /* UmiRunConfig API: load + argv/envp construction   */
#include "build_runner.h"         /* UmiBuildRunner API: process spawn + callbacks     */
#include "diagnostics_router.h"   /* UmiDiagRouter API: feed lines + begin/end         */

/*-----------------------------------------------------------------------------
 * umi_run_pipeline_start
 *
 * PURPOSE:
 *   Load the current run configuration, prepare argv/envp/cwd, start the child
 *   process using the runner, and connect output lines to diagnostics/UI.
 *
 * PARAMETERS:
 *   out   - (in) Output pane sink for mirrored lines.
 *   plist - (in) Problem list that will receive parsed diagnostics.
 *   err   - (out) Standard GLib error reporting (may be NULL).
 *
 * RETURNS:
 *   TRUE on success (process started); FALSE on failure with 'err' set.
 *---------------------------------------------------------------------------*/
gboolean umi_run_pipeline_start(struct _UmiOutputPane *out,
                                struct _UmiProblemList *plist,
                                GError **err);

/*-----------------------------------------------------------------------------
 * umi_run_pipeline_stop
 *
 * PURPOSE:
 *   Politely request the active child process (if any) to stop. This cancels
 *   pending async I/O and asks the runner to terminate the process.
 *---------------------------------------------------------------------------*/
void     umi_run_pipeline_stop(void);

#endif /* UMICOM_RUN_PIPELINE_H */
/*--- end of file ---*/
