/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/msys_env.h
 * PURPOSE: Detect MSYS/MinGW and construct PATH hints
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MSYS_ENV_H
#define UMICOM_MSYS_ENV_H

#include <glib.h>

/* Returns a newly allocated PATH hint string for MSYS/MinGW environments,
 * or NULL if not detected. */
gchar *umi_msys_path_hint(void);

#endif /* UMICOM_MSYS_ENV_H */
