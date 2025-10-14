/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/include/runner.h
 *
 * PROJECT:
 *   Umicom Studio IDE — public, cross-module process runner API.
 *
 * PURPOSE:
 *   Provide a tiny, stable interface for any module (e.g., core, GUI, plugins)
 *   to spawn and control external processes (build, run, tools) without knowing
 *   anything about the underlying implementation. This keeps modules decoupled:
 *   only this header is included across modules; implementation lives elsewhere.
 *
 * HOW IT WORKS (HIGH LEVEL):
 *   - You create an opaque UmiRunner handle with umi_runner_new().
 *   - You launch a process with umi_runner_run(), passing argv/envp/cwd.
 *   - You may stop it with umi_runner_stop() and finally free with
 *     umi_runner_free().
 *   - You can optionally receive an exit callback when the child terminates.
 *
 * DESIGN CHOICES:
 *   - Pure C and GLib types (gboolean, GError) which we already use widely.
 *   - Opaque struct (UmiRunner) hides implementation details for loose coupling.
 *   - argv/envp are standard NULL-terminated vectors (C conventions).
 *   - No GTK dependencies — this API is UI-agnostic.
 *
 * SAFETY / OWNERSHIP:
 *   - UmiRunner* is owned by the caller; free with umi_runner_free().
 *   - argv/envp are borrowed (not modified); you keep ownership and free them.
 *   - Error reporting uses the standard GLib out-param pattern (GError**).
 *
 * PREREQUISITES:
 *   - GLib development headers (for gboolean, gpointer, GError).
 *
 * USAGE (PSEUDOCODE):
 *   // argv: { "bash", "-lc", "echo hi", NULL }
 *   UmiRunner *r = umi_runner_new();                              // create handle
 *   gboolean ok = umi_runner_run(r, argv, NULL, NULL, on_exit,    // launch child
 *                                NULL, &err);
 *   if(!ok) { log(err->message); g_clear_error(&err); }           // handle errors
 *   // ... later ...
 *   umi_runner_stop(r);                                           // ask to stop
 *   umi_runner_free(r);                                           // release handle
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab
 * Date: 2025-10-12 | License: MIT
 *---------------------------------------------------------------------------*/

#pragma once

#include <glib.h>

G_BEGIN_DECLS

typedef struct UmiRunner UmiRunner;  /* Opaque handle; fields are hidden.      */

/* Exit callback: invoked once when the child exits. */
typedef void (*UmiRunnerExitCb)(gpointer user_data, int exit_code);

/* Constructor */
UmiRunner *umi_runner_new(void);

/* Spawn a child process (non-blocking). Returns TRUE on spawn success. */
gboolean umi_runner_run(
  UmiRunner                 *r,
  const char * const        *argv,     /* corrected const-qualification       */
  const char * const        *envp,     /* corrected const-qualification       */
  const char                *cwd,
  UmiRunnerExitCb            on_exit,
  gpointer                   user_data,
  GError                   **error
);

/* Request polite termination of the running child (if any). */
void umi_runner_stop(UmiRunner *r);

/* Destructor */
void umi_runner_free(UmiRunner *r);

G_END_DECLS
/*  END OF FILE */