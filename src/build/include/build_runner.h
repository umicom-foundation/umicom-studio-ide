/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/include/build_runner.h
 * PURPOSE: Interfaces for the build runner (spawns and monitors build tasks).
 * NOTE:    This header MUST NOT force GUI includes. We forward-declare
 *          UmiOutputPane so the type can be referenced without pulling in
 *          <gtk/gtk.h>. C files that need the real API should include
 *          "src/panes/output/include/output_pane.h".
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_RUNNER_H         /* Include guard start */
#define UMICOM_BUILD_RUNNER_H         /* Mark guard defined */

/* Keep headers minimal here to avoid leaking GUI deps into non-GUI builds. */
#include <stddef.h>                   /* size_t for completeness */
#ifdef __cplusplus
extern "C" {
#endif
/* Forward declarations (break circular / heavy deps) */
typedef struct _UmiOutputPane UmiOutputPane;   /* Opaque console/output pane */
typedef struct _UmiBuildRunner UmiBuildRunner; /* Opaque build runner object */

/*-----------------------------------------------------------------------------
 * Lifecycle
 *---------------------------------------------------------------------------*/

/* Create a runner that will report to the given output pane (may be NULL). */
UmiBuildRunner *umi_build_runner_new(UmiOutputPane *out);

/* Free the runner and any internal resources. Safe on NULL. */
void            umi_build_runner_free(UmiBuildRunner *runner);

/*-----------------------------------------------------------------------------
 * Control
 *---------------------------------------------------------------------------*/

/* Start a build job (e.g., run a command line). Returns 0 on success. */
int             umi_build_runner_start(UmiBuildRunner *runner,
                                       const char     *cmdline,
                                       const char     *workdir);

/* Request termination of the running job (best-effort). */
void            umi_build_runner_kill(UmiBuildRunner *runner);

/* Check if a job is currently running (non-zero = running). */
int             umi_build_runner_is_running(const UmiBuildRunner *runner);

/* Optional: set a user pointer for callbacks (stored, not owned). */
void            umi_build_runner_set_user(UmiBuildRunner *runner, void *user);

/* Optional: get the user pointer back. */
void           *umi_build_runner_get_user(const UmiBuildRunner *runner);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* UMICOM_BUILD_RUNNER_H */    /* Include guard end */
