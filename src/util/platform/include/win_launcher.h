/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/win_launcher.h
 * PURPOSE: Windows/MSYS launcher using PATH hints from msys_env
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WIN_LAUNCHER_H
#define UMICOM_WIN_LAUNCHER_H

#include <gio/gio.h>
#include "../../sys/include/msys_env.h"

/* Launch a command with optional cwd; on Windows, prepend MSYS/MinGW PATH hints if available. */
GSubprocess* umi_win_spawn_with_msys_env(const char *cwd, char * const *argv, GError **err);

#endif /* UMICOM_WIN_LAUNCHER_H */
