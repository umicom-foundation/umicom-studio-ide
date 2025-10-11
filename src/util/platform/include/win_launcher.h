/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/platform/include/win_launcher.h
 * PURPOSE: Windows/MSYS launcher using PATH hints from msys_env
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WIN_LAUNCHER_H
#define UMICOM_WIN_LAUNCHER_H

#include <gio/gio.h>           /* For GSubprocess / GSubprocessLauncher types */

/* NOTE:
 * We depend on msys_env utilities ONLY for behavior on Windows to prepend
 * PATH hints. Include via project include-dirs (no brittle ../../ paths). */
#include <msys_env.h>          /* umi_msys_path_hint() */

/*-----------------------------------------------------------------------------
 * umi_win_spawn_with_msys_env
 *
 * PURPOSE:
 *   Spawn a child process (argv[0] is the program) using GLib.
 *   On Windows, if msys_env reports a PATH hint, we prepend it to PATH
 *   for the child so MSYS/MinGW tools are found reliably.
 *
 * PARAMETERS:
 *   cwd   - Optional working directory for the spawned process (NULL = inherit).
 *   argv  - NULL-terminated vector: argv[0]=program, argv[1..]=args, argv[n]=NULL.
 *   err   - (out) GLib error for detailed diagnostics on failure.
 *
 * RETURNS:
 *   GSubprocess* on success (caller owns reference via g_object_unref),
 *   or NULL on failure (err set).
 *
 * LIFETIME NOTES:
 *   - The argv vector is not consumed; caller retains ownership.
 *   - The returned GSubprocess must be unreffed by caller.
 *---------------------------------------------------------------------------*/
GSubprocess *umi_win_spawn_with_msys_env(const char *cwd,
                                         char * const *argv,
                                         GError **err);

#endif /* UMICOM_WIN_LAUNCHER_H */
