/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/entry/cli_entry.c
 *
 * PURPOSE:
 *   Command-line entry helpers for headless / console mode. Configures
 *   logging and prints a tiny banner so the CLI can run tools without the GUI.
 *
 * DESIGN:
 *   - Dependency-light, pure C.
 *   - Optional project logger via __has_include; otherwise GLib shim.
 *   - Defensive argument handling; no unbounded prints.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>
#include <stdio.h>
#include <string.h>

/* Make __has_include portable for C toolchains that lack it. */
#ifndef __has_include
#  define __has_include(x) 0
#endif

/* Prefer project logger; otherwise provide safe fallbacks to GLib. */
#if __has_include("umi_log.h")
#  include "umi_log.h"
#elif __has_include("core/umi_log.h")
#  include "core/umi_log.h"
#else
#  ifndef UMI_LOG_DEBUG
#    define UMI_LOG_DEBUG G_LOG_LEVEL_DEBUG
#  endif
#  ifndef UMI_LOG_INFO
#    define UMI_LOG_INFO  G_LOG_LEVEL_INFO
#  endif
#  ifndef UMI_LOG_WARN
#    define UMI_LOG_WARN  G_LOG_LEVEL_WARNING
#  endif
#  ifndef UMI_LOG_ERROR
#    define UMI_LOG_ERROR G_LOG_LEVEL_CRITICAL
#  endif
static void umi_log_set_level(int lvl) { (void)lvl; }
#  ifndef UMI_LOGD
#    define UMI_LOGD(fmt, ...) g_log("umicom", G_LOG_LEVEL_DEBUG, (fmt), ##__VA_ARGS__)
#  endif
#  ifndef UMI_LOGI
#    define UMI_LOGI(fmt, ...) g_log("umicom", G_LOG_LEVEL_INFO,  (fmt), ##__VA_ARGS__)
#  endif
#  ifndef UMI_LOGW
#    define UMI_LOGW(fmt, ...) g_log("umicom", G_LOG_LEVEL_WARNING,(fmt), ##__VA_ARGS__)
#  endif
#  ifndef UMI_LOGE
#    define UMI_LOGE(fmt, ...) g_log("umicom", G_LOG_LEVEL_CRITICAL,(fmt), ##__VA_ARGS__)
#  endif
#endif

static void configure_logging_level(int log_level, int verbose)
{
    (void)verbose;
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

    const char *ws = (workspace && workspace[0]) ? workspace : "(none)";
    const size_t ws_len = strlen(ws);
    if (ws_len > 1024)
        UMI_LOGW("Workspace path is extremely long (%zu chars); truncating in log.", ws_len);

    UMI_LOGI("Umicom Studio (CLI) starting — headless=%d, workspace='%.*s'",
             headless, (int)((ws_len > 1024) ? 1024 : ws_len), ws);
    UMI_LOGD("Verbose level: %d; log level code: %d", verbose, log_level);
}
