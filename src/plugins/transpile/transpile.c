/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/plugins/transpile/transpile.c
 * PURPOSE: Demonstration 'transpile' / translate plugin wiring
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#include <glib.h>
#include "status_util.h"  /* logging helpers */

/* Detect optional uengine LLM headers. */
#if defined(__has_include)
  #if __has_include(<ueng/llm.h>)
    #define USIDE_HAVE_UENGINE 1
    #include <ueng/llm.h>
  #endif
#endif

/* Local translation helper provided by i18n_translate.c */
gchar *umi_translate_text(const gchar *text,
                          const gchar *source_lang,
                          const gchar *target_lang,
#ifdef USIDE_HAVE_UENGINE
                          UmiLlmCfg   *cfg,
#else
                          void        *cfg,
#endif
                          GError     **err);

/*---------------------------------------------------------------------------
 * umi_transpile_demo:
 *   Example entrypoint that translates a fixed snippet to English. In a
 *   future iteration this will dispatch a full transpilation pipeline.
 *---------------------------------------------------------------------------*/
void umi_transpile_demo(void) {
  const gchar *sample = "مرحبا بالعالم"; /* Arabic 'Hello, world' */
  GError *err = NULL;

#ifdef USIDE_HAVE_UENGINE
  UmiLlmCfg cfg = {0};
  umi_llm_cfg_init_from_env(&cfg);
  g_autofree gchar *out = umi_translate_text(sample, "ar", "en", &cfg, &err);
#else
  g_autofree gchar *out = umi_translate_text(sample, "ar", "en", NULL, &err);
#endif

  if (!out) {
    if (err) {
      umi_status_error("Transpile", "Translation failed: %s", err->message);
      g_error_free(err);
    } else {
      umi_status_error("Transpile", "Translation failed (unknown error).");
    }
    return;
  }

  umi_status_info("Transpile", "Translation result: %s", out);
}
