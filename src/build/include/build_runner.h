/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/include/build_runner.h
 * PURPOSE: Public API for running external build tasks asynchronously using
 *          GLib/GIO. The implementation lives in build_runner.c.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_RUNNER_H
#define UMICOM_BUILD_RUNNER_H

/* GLib/GIO provide the base types (gboolean, gpointer) and error model (GError). */
#include <glib.h>     /* basic types/macros (gboolean, gpointer, etc.) */
#include <gio/gio.h>  /* GError forward decls + I/O types used in signatures */

/* Opaque struct; defined privately in build_runner.c */
typedef struct _UmiBuildRunner UmiBuildRunner;

/* Callback invoked when the process exits; 'code' is the exit status (or -1 on error). */
typedef void (*UmiBuildExitCb)(gpointer user, int code);

/* Generic output sink for lines produced by the child process.
 * - 'user' is the sink’s user-data
 * - 'line' is a NUL-terminated UTF-8 string (without trailing newline)
 * - 'is_err' is TRUE if the line came from stderr, FALSE if from stdout
 */
typedef void (*UmiOutputSink)(gpointer user, const char *line, gboolean is_err);

/* Create a new runner (owns its GIO state). */
UmiBuildRunner *umi_build_runner_new(void);

/* Provide a sink that will receive output lines (optional). */
void umi_build_runner_set_sink(UmiBuildRunner *br, UmiOutputSink sink, gpointer sink_user);

/* Start the process asynchronously:
 * - argv: NULL-terminated vector of program and args (argv[0] is the program)
 * - envp: optional NULL-terminated environment (pass NULL to inherit)
 * - cwd : optional working directory (pass NULL to use current)
 * - on_exit/user: optional exit callback + user-data
 * - err : (out) GLib error (set on failure)
 *
 * Returns TRUE on successful spawn (async I/O begins), FALSE on failure.
 */
gboolean umi_build_runner_run(UmiBuildRunner *br,
                              char * const *argv,
                              char * const *envp,
                              const char *cwd,
                              UmiBuildExitCb on_exit,
                              gpointer user,
                              GError **err);

/* Request graceful stop (cancels async reads and forces child exit). */
void umi_build_runner_stop(UmiBuildRunner *br);

/* Destroy the runner and release resources. */
void umi_build_runner_free(UmiBuildRunner *br);

#endif /* UMICOM_BUILD_RUNNER_H */
