/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/include/env.h
 * PURPOSE: Environment helpers (paths, configs, platform conditionals)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_ENV_H
#define UMICOM_ENV_H

#include <glib.h>    /* gchar, GError, gsize, ownership semantics */

/*-----------------------------------------------------------------------------
 * umi_env_is_windows / umi_env_is_unix
 *
 * PURPOSE:
 *   Tiny, compile-time driven helpers to branch platform-specific behavior
 *   at runtime without sprinkling #ifdefs through higher-level code.
 *---------------------------------------------------------------------------*/
gboolean umi_env_is_windows(void);
gboolean umi_env_is_unix(void);

/*-----------------------------------------------------------------------------
 * umi_env_app_config_dir
 *
 * PURPOSE:
 *   Return the directory where application config should be stored
 *   (e.g. %APPDATA%\Umicom\Studio on Windows, ~/.config/umicom-studio on Unix).
 *
 * RETURNS:
 *   Newly allocated path string (g_free when done), or NULL on failure.
 *---------------------------------------------------------------------------*/
gchar *umi_env_app_config_dir(void);

/*-----------------------------------------------------------------------------
 * umi_env_data_dir
 *
 * PURPOSE:
 *   Return the directory for application data/assets (read-only at runtime),
 *   following platform conventions (e.g., ProgramData/… on Windows).
 *
 * RETURNS:
 *   Newly allocated path string (g_free), or NULL on failure.
 *---------------------------------------------------------------------------*/
gchar *umi_env_data_dir(void);

/*-----------------------------------------------------------------------------
 * umi_env_expand_vars
 *
 * PURPOSE:
 *   Expand ${VAR} and %VAR% style environment references within 'input'
 *   into a newly allocated string. Supports both Windows and Unix syntax.
 *
 * RETURNS:
 *   Newly allocated string (g_free), or NULL on error.
 *---------------------------------------------------------------------------*/
gchar *umi_env_expand_vars(const gchar *input, GError **err);

#endif /* UMICOM_ENV_H */
