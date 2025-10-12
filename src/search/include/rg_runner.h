/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/search/include/rg_runner.h
 * PURPOSE: Thin wrapper to run ripgrep with our argument builder
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#pragma once
#include <glib.h>

/*---------------------------------------------------------------------------
 * umi_rg_run:
 *   Execute ripgrep with the provided argv vector (NULL-terminated).
 *   Captures stdout and stderr into GString buffers (which must be pre-
 *   allocated by the caller). Returns TRUE on successful spawn; the exit
 *   code from ripgrep is written into *exit_status when non-NULL.
 *
 *   Ownership: caller retains buffers and must free them.
 *---------------------------------------------------------------------------*/
gboolean umi_rg_run(char **argvv, GString *out, GString *err, int *exit_status);
/*---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * End of File
 *---------------------------------------------------------------------------*/