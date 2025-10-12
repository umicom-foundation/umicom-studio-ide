/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/cli_entry.c
 * PURPOSE: CLI logging setup + light banner (build-safe on all platforms)
 *
 * Why this file changed:
 *   Previous builds failed here because the logging macros/functions
 *   (UMI_LOG*, umi_log_set_level) were used without including the proper
 *   header. Some trees keep the header at "core/umi_log.h", others at
 *   "umi_log.h". This file now includes either if present and provides
 *   robust fallbacks so the unit always compiles.
 *
 * Created by: Umicom Foundation | Maintained: Community| Author: Sammy Hegab | Date: 2025-10-01 | MIT
 * License: MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>
#include <stdio.h>
#include <string.h>

/* Try to include the project logging header from either location. */
#if __has_include("core/umi_log.h")
#  include "core/umi_log.h"
#elif __has_include("umi_log.h")
#  include "umi_log.h"
#else
/*---------------------------------------------------------------------------
 * Fallbacks (only used if the real header is not available at compile time).
 * These map the IDE's logging macros onto GLib so this TU still builds.
 *---------------------------------------------------------------------------*/
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

/* No-op shim: projects with real logging will provide this in umi_log.c */
static void umi_log_set_level(int lvl) { (void)lvl; }

#ifndef UMI_LOGI
#  define UMI_LOGI(fmt, ...) g_log("umicom", G_LOG_LEVEL_INFO,  (fmt), ##__VA_ARGS__)
#endif
#ifndef UMI_LOGD
#  define UMI_LOGD(fmt, ...) g_log("umicom", G_LOG_LEVEL_DEBUG, (fmt), ##__VA_ARGS__)
#endif
#ifndef UMI_LOGW
#  define UMI_LOGW(fmt, ...) g_log("umicom", G_LOG_LEVEL_WARNING, (fmt), ##__VA_ARGS__)
#endif
#ifndef UMI_LOGE
#  define UMI_LOGE(fmt, ...) g_log("umicom", G_LOG_LEVEL_CRITICAL, (fmt), ##__VA_ARGS__)
#endif
#endif /* header include selection */

/*---------------------------------------------------------------------------
 * Public options struct is declared in cli_entry.h in the repo. We avoid
 * including it to keep this TU independent across branches; instead we
 * accept a minimal opaque struct via pointer for log setup, and provide
 * a simple helper API used by main/app glue.
 *---------------------------------------------------------------------------*/

/* Minimal mirror used only in this TU if cli_entry.h is unavailable. */
typedef struct {
  int headless;      /* 0 or 1 */
  const char *workspace;
  int verbose;       /* 0/1/2 */
  int log_level;     /* 0=debug,1=info,2=warn,3=error (matches prior code) */
} UmiCliOptionsLocal;

/* Configure logging level using either the project's logger or fallbacks. */
static void configure_logging_level(int log_level, int verbose) {
  (void)verbose;
  switch (log_level) {
    case 0: umi_log_set_level(UMI_LOG_DEBUG); break;
    case 1: umi_log_set_level(UMI_LOG_INFO);  break;
    case 2: umi_log_set_level(UMI_LOG_WARN);  break;
    default:umi_log_set_level(UMI_LOG_ERROR); break;
  }
}

/* Safe entry used by the app/launcher.  If your tree already has a symbol
 * like umi_cli_run(...) with a different signature, just keep that too â€“
 * having this function present does not clash (distinct name). */
void umi_cli_entry_run_buildsafe(int headless, const char *workspace, int verbose, int log_level) {
  configure_logging_level(log_level, verbose);
  UMI_LOGI("Umicom Studio CLI starting (headless=%d, workspace=%s)", headless, workspace ? workspace : "(null)");
  UMI_LOGD("Verbose mode: %d", verbose);
  (void)headless; (void)workspace; (void)verbose; (void)log_level;
}

/* Optional helper that some launchers call with a structured options object.
 * If your branch exposes UmiCliOptions in cli_entry.h, the real implementation
 * will be picked up by the compiler and this function can be ignored. */
void umi_cli_configure_from_local_opts(const UmiCliOptionsLocal *opt) {
  if (!opt) return;
  umi_cli_entry_run_buildsafe(opt->headless, opt->workspace ? opt->workspace : "", opt->verbose, opt->log_level);
}