/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/include/build_runner.h
 * PURPOSE:
 *   Launch external processes (build/tools) via GLib/GIO (GSubprocess),
 *   stream stdout/stderr lines asynchronously to a sink, and notify when
 *   the process exits. Opaque handle; UI-agnostic; pure C.
 *
 * LOOSEN COUPLING:
 *   - The struct is opaque; only this header is the cross-module contract.
 *   - No GTK includes here; only GLib/GIO base types appear in the API.
 *
 * SAFETY / OWNERSHIP:
 *   - Caller owns UmiBuildRunner* and frees it with umi_build_runner_free().
 *   - argv/envp are borrowed (NULL-terminated); not modified by the runner.
 *   - If no envp is provided (NULL), the child inherits the current env.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_RUNNER_H
#define UMICOM_BUILD_RUNNER_H

#include <glib.h>  /* gboolean, gpointer, GError, etc. */
#include <gio/gio.h> /* GSubprocess APIs (only in prototypes; keeps it accurate) */

/* Opaque handle (internals private to build_runner.c).                        */
typedef struct _UmiBuildRunner UmiBuildRunner;

/* Exit callback: invoked once when the child exits.                           */
typedef void (*UmiBuildExitCb)(gpointer user, int exit_code);

/* Output sink: receives each line from stdout/stderr with a flag.             */
typedef void (*UmiOutputSink)(gpointer user, const char *line, gboolean is_err);

/* Construct / destroy.                                                        */
UmiBuildRunner *umi_build_runner_new(void);
void            umi_build_runner_free(UmiBuildRunner *br);

/* Optional sink wiring (pass NULL to disable routing).                        */
void            umi_build_runner_set_sink(UmiBuildRunner *br,
                                          UmiOutputSink   sink,
                                          gpointer        sink_user);

/* Run a child process.
 * argv: { program, arg1, ..., NULL }   (NULL-terminated)
 * envp: { "K=V", ..., NULL } or NULL to inherit environment.
 * cwd : working directory or NULL to use current directory.
 *
 * NOTE:
 *   GLib expects a **mutable** GStrv for set_environ(), so the implementation
 *   duplicates envp internally as needed (your original envp is not modified).
 */
gboolean        umi_build_runner_run(UmiBuildRunner     *br,
                                     char * const *      argv,
                                     char * const *      envp,
                                     const char *        cwd,
                                     UmiBuildExitCb      on_exit,
                                     gpointer            user,
                                     GError            **err);

/* Best-effort stop: cancels async I/O and asks process to exit.               */
void            umi_build_runner_stop(UmiBuildRunner *br);

#endif /* UMICOM_BUILD_RUNNER_H */
/*---------------------------------------------------------------------------*/
