/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build_runner.h
 * PURPOSE: Start/stop a subprocess; stream stdout/stderr to Output pane
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_RUNNER_H
#define UMICOM_BUILD_RUNNER_H

#include <gio/gio.h>
#include "../../panes/output/include/output_pane.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _UmiBuildRunner UmiBuildRunner;

/* On process exit, called with exit code. */
typedef void (*UmiBuildExitCb)(gpointer user, int exit_code);

/* Create a runner which writes to the provided output pane (may be NULL). */
UmiBuildRunner *umi_build_runner_new(UmiOutputPane *out);

/* Start the process with cwd and argv (NULL-terminated). Returns TRUE on spawn success.
 * Exit notifications are delivered via 'on_exit'. */
gboolean umi_build_runner_run(UmiBuildRunner *br,
                              const char *cwd,
                              char * const *argv,
                              UmiBuildExitCb on_exit,
                              gpointer user,
                              GError **err);

/* Attempt polite stop: sends SIGTERM on POSIX or terminates process on Windows. */
void     umi_build_runner_stop(UmiBuildRunner *br);

/* Free resources (does not kill a running process; call stop() first). */
void     umi_build_runner_free(UmiBuildRunner *br);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UMICOM_BUILD_RUNNER_H */
