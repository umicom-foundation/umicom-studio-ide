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

#pragma once                      /* Ensure this header is included only once.  */

#include <glib.h>                 /* Import GLib core types (gboolean, GError). */

/*-----------------------------------------------------------------------------
 * Forward declaration:
 *   UmiRunner is intentionally opaque. Callers hold only a pointer to it and
 *   interact exclusively via the functions below. Implementation details are
 *   private to the module that defines them (e.g., src/build/*).
 *---------------------------------------------------------------------------*/
typedef struct UmiRunner UmiRunner;  /* Opaque handle; fields are hidden.      */

/*-----------------------------------------------------------------------------
 * Exit callback type:
 *   Invoked exactly once when the child process exits (normally or abnormally).
 *
 * PARAMETERS:
 *   user_data  - the same pointer you passed to umi_runner_run().
 *   exit_code  - the process exit status as an integer (platform-normalized).
 *---------------------------------------------------------------------------*/
typedef void (*UmiRunnerExitCb)(gpointer user_data, int exit_code);  /* cb type */

/*-----------------------------------------------------------------------------
 * umi_runner_new
 *
 * PURPOSE:
 *   Allocate and initialize a new runner handle. The handle encapsulates any
 *   state (thread pools, IO hooks, etc.) that the implementation needs.
 *
 * RETURNS:
 *   UmiRunner*  - a valid runner handle on success; NULL only on fatal setup
 *                 failure (rare).
 *---------------------------------------------------------------------------*/
UmiRunner *umi_runner_new(void);                             /* constructor     */

/*-----------------------------------------------------------------------------
 * umi_runner_run
 *
 * PURPOSE:
 *   Spawn a new child process using argv/envp/cwd. This call is non-blocking
 *   (it returns after the process is launched). When the child exits, the
 *   optional 'on_exit' callback is invoked.
 *
 * PARAMETERS:
 *   r         - (in) runner handle created by umi_runner_new(); must be non-NULL.
 *   argv      - (in) NULL-terminated vector of program/args (argv[0] = program).
 *   envp      - (in) NULL-terminated environment vector (key=value strings),
 *               or NULL to inherit the parent process environment.
 *   cwd       - (in) working directory for the child, or NULL to inherit.
 *   on_exit   - (in) optional callback invoked when the process terminates;
 *               pass NULL if you don’t need a notification.
 *   user_data - (in) opaque pointer passed through to 'on_exit' unchanged.
 *   error     - (out) GLib error (set on failure); may be NULL if you don’t
 *               need error details.
 *
 * RETURNS:
 *   TRUE  - the process was successfully spawned.
 *   FALSE - spawning failed; if 'error' is non-NULL it contains details.
 *
 * NOTES:
 *   - The runner does NOT take ownership of argv/envp; you remain responsible
 *     for freeing them when appropriate.
 *   - The implementation must not modify argv/envp contents.
 *---------------------------------------------------------------------------*/
gboolean umi_runner_run(
  UmiRunner           *r,          /* runner handle (required)                  */
  char * const        *argv,       /* program + args (NULL-terminated)          */
  char * const        *envp,       /* environment (NULL => inherit)             */
  const char          *cwd,        /* working directory (NULL => inherit)       */
  UmiRunnerExitCb      on_exit,    /* optional exit callback                     */
  gpointer             user_data,  /* opaque pointer passed to callback          */
  GError             **error       /* error out (may be NULL)                    */
);                                  /* returns TRUE on spawn success             */

/*-----------------------------------------------------------------------------
 * umi_runner_stop
 *
 * PURPOSE:
 *   Request a graceful stop for the currently running child process (if any).
 *   The exact semantics are implementation-defined (e.g., send SIGTERM on
 *   POSIX / terminate process on Windows, cancel IO, etc.), but the goal is
 *   to end the child cleanly.
 *
 * PARAMETERS:
 *   r - (in) runner handle; NULL is tolerated as a no-op.
 *---------------------------------------------------------------------------*/
void umi_runner_stop(UmiRunner *r);        /* polite termination request         */

/*-----------------------------------------------------------------------------
 * umi_runner_free
 *
 * PURPOSE:
 *   Destroy the runner handle and release all associated resources. If a child
 *   is still running, the implementation should attempt to stop it first or
 *   document its behavior (best effort graceful shutdown).
 *
 * PARAMETERS:
 *   r - (in) runner handle to destroy; NULL is allowed (no-op).
 *---------------------------------------------------------------------------*/
void umi_runner_free(UmiRunner *r);        /* destructor                          */
/*--- end of file ---*/