/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/env.h
 * PURPOSE: Helpers to convert a KEY=VALUE multi-line block into an envp array.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_ENV_H
#define UMICOM_ENV_H

#include <glib.h>

/* Parse a newline-separated list of KEY=VALUE entries into a NULL-terminated
 * envp vector suitable for GSubprocess. Caller owns returned array and each
 * element (free with g_strfreev). Empty input yields NULL. */
gchar **umi_env_from_multiline(const char *env_multiline);

#endif /* UMICOM_ENV_H */
