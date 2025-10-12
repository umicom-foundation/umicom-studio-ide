/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/plugins/transpile/transpile.c
 * PURPOSE:
 *   Minimal “Transpile” demo showcasing umi_translate_text(...).
 *
 * DESIGN:
 *   - No mandatory UI dependencies. If a status UI is available, define
 *     UMI_HAVE_STATUS_UI and include its header to get rich messages.
 *   - Otherwise we fall back to g_warning/g_message (no compile break).
 *   - Uses umi_llm_cfg_init_from_env(...) so it Just Works with env vars.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-01
 * License: MIT
 *---------------------------------------------------------------------------*/

#include "transpile.h"
#include <glib.h>
#include <llm.h>

/* ------------------------------------------------------------------------- */
/* Optional status UI glue.
 * If your tree provides these helpers, enable by adding
 *   add_definitions(-DUMI_HAVE_STATUS_UI)
 * and including the correct header below (example path shown).
 */
#ifdef UMI_HAVE_STATUS_UI
#  include "ui/status_bar.h" /* must declare umi_status_error/info */
#else
#  define umi_status_error(tag, fmt, ...)  g_warning ("[%s] " fmt, tag, __VA_ARGS__)
#  define umi_status_info(tag,  fmt, ...)  g_message ("[%s] " fmt, tag, __VA_ARGS__)
#endif
/* ------------------------------------------------------------------------- */

void
umi_transpile_demo(void)
{
  /* Sample text (English) -> Arabic just as a smoke-test. */
  const char *sample = "Hello world! This translation is powered by Umicom Studio IDE.";

  /* Initialize LLM configuration from environment. */
  UmiLlmCfg cfg = {0};
  umi_llm_cfg_init_from_env(&cfg);

  /* Error buffer per our API contract. */
  char err[256] = {0};

  g_autofree gchar *translated =
      umi_translate_text(sample, "en", "ar", &cfg, err, sizeof(err));

  if (!translated) {
    /* No UI coupling required: macro falls back to g_warning if no status UI. */
    umi_status_error("Transpile", "Translation failed: %s", err[0] ? err : "unknown error");
    return;
  }

  umi_status_info("Transpile", "Translation result: %s", translated);

  /* Done: translated is g_free()’d automatically because of g_autofree. */
}
