/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/entry/cli_entry.c
 *
 * PURPOSE:
 *   Command-line entry helpers for headless / console mode. Configures
 *   logging and prints a small banner so the CLI can run tools or scripts
 *   without pulling the full GUI. This translation unit is intentionally
 *   tiny and dependency-light so it compiles on every platform/layout.
 *
 * DESIGN:
 *   - Includes project logging by name if available; otherwise provides safe
 *     no-op fallbacks mapped to GLib's logging.
 *   - Avoids any deep/relative includes; header names only.
 *   - All arguments are validated and bounded to avoid misuse.
 *
 * API:
 *   void umi_cli_entry_run_buildsafe(int headless,
 *                                    const char *workspace,
 *                                    int verbose,
 *                                    int log_level);
 *
 * NOTES:
 *   • This file never owns the 'workspace' string; it just logs it.
 *   • If your tree exposes richer CLI structs, this TU remains compatible.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>     /* g_log, g_message, g_strcmp0 */
#include <stdio.h>    /* printf for simple banners    */
#include <string.h>   /* strlen safety guards         */

/* Try to include the project's logging API; otherwise fall back. */
#if __has_include("umi_log.h")
#  include "umi_log.h"           /* expected to define umi_log_set_level + macros */
#elif __has_include("core/umi_log.h")
#  include "core/umi_log.h"
#else
/*--------------------------------------------------------------------------
 * Fallback shim: if the real header is not present in this layout, define
 * a minimal adapter so this unit still compiles and emits logs via GLib.
 *--------------------------------------------------------------------------*/
#ifndef UMI_LOG_DEBUG
#  define UMI_LOG_DEBUG G_LOG_LEVEL_DEBUG
#endif
#ifndef UMI_LOG_INFO
#  define UMI_LOG_INFO  G_LOG_LEVEL_INFO
#endif
#ifndef UMI_LOG_WARN
#  define UMI_LOG_WARN  G_LOG_LEVEL_WARNING
#endif
#ifndef UMI_LOG_ERROR
#  define UMI_LOG_ERROR G_LOG_LEVEL_CRITICAL
#endif
static void umi_log_set_level(int lvl) { (void)lvl; } /* no-op if missing */

#ifndef UMI_LOGD
#  define UMI_LOGD(fmt, ...) g_log("umicom", G_LOG_LEVEL_DEBUG, (fmt), ##__VA_ARGS__)
#endif
#ifndef UMI_LOGI
#  define UMI_LOGI(fmt, ...) g_log("umicom", G_LOG_LEVEL_INFO,  (fmt), ##__VA_ARGS__)
#endif
#ifndef UMI_LOGW
#  define UMI_LOGW(fmt, ...) g_log("umicom", G_LOG_LEVEL_WARNING,(fmt), ##__VA_ARGS__)
#endif
#ifndef UMI_LOGE
#  define UMI_LOGE(fmt, ...) g_log("umicom", G_LOG_LEVEL_CRITICAL,(fmt), ##__VA_ARGS__)
#endif
#endif /* logging include selection */

/* Map the numeric log level used by existing code to the chosen logger. */
static void configure_logging_level(int log_level, int verbose)
{
    (void)verbose; /* Reserved for future structured verbosity */
    switch (log_level) {
        case 0: umi_log_set_level(UMI_LOG_DEBUG); break;
        case 1: umi_log_set_level(UMI_LOG_INFO);  break;
        case 2: umi_log_set_level(UMI_LOG_WARN);  break;
        default:umi_log_set_level(UMI_LOG_ERROR); break;
    }
}

/* Public helper used by launcher code; lightweight and safe. */
void umi_cli_entry_run_buildsafe(int headless, const char *workspace, int verbose, int log_level)
{
    configure_logging_level(log_level, verbose);

    /* Guard 'workspace' for logging — avoid NULL deref and huge noisy strings. */
    const char *ws = (workspace && workspace[0]) ? workspace : "(none)";
    const size_t ws_len = strlen(ws);
    if (ws_len > 1024) { /* prevent pathological terminal spam */
        UMI_LOGW("Workspace path is extremely long (%zu chars); truncating for log output.", ws_len);
    }

    UMI_LOGI("Umicom Studio (CLI) starting — headless=%d, workspace='%.*s'",
             headless, (int)((ws_len > 1024) ? 1024 : ws_len), ws);

    UMI_LOGD("Verbose level: %d; log level code: %d", verbose, log_level);

    /* If you need to kick a default command, do it here (kept empty by design). */
}
