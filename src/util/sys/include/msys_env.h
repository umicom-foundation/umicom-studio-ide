/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/include/msys_env.h
 * PURPOSE: Discover MSYS/MinGW roots and build PATH hints for child procs
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MSYS_ENV_H
#define UMICOM_MSYS_ENV_H

#include <glib.h>  /* gchar, gboolean */

/*-----------------------------------------------------------------------------
 * umi_msys_detected
 *
 * PURPOSE:
 *   Return TRUE when MSYS/MinGW is detected on this system (Windows only).
 *   On Unix this generally returns FALSE.
 *---------------------------------------------------------------------------*/
gboolean umi_msys_detected(void);

/*-----------------------------------------------------------------------------
 * umi_msys_path_hint
 *
 * PURPOSE:
 *   Produce a semicolon-separated PATH prefix containing MSYS/MinGW locations
 *   (e.g., "C:\\msys64\\usr\\bin;C:\\msys64\\mingw64\\bin") for child processes.
 *
 * RETURNS:
 *   Newly allocated string (g_free) or NULL if MSYS not detected.
 *---------------------------------------------------------------------------*/
gchar *umi_msys_path_hint(void);

#endif /* UMICOM_MSYS_ENV_H */
