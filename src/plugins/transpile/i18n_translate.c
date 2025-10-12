/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/plugins/transpile/i18n_translate.c
 * PURPOSE: Simple LLM-backed translation helper used by the Transpile plugin
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
/* NOTE:
 * This module is written to compile both with and without the optional
 * 'uengine' LLM dependency. When the headers from
 * third_party/umicom/uengine/include/ueng are available, we enable the
 * real implementation. Otherwise, we build a graceful stub that reports
 * that translation is unavailable.
 */

#include <glib.h>

/* Detect availability of uengine public headers at compile time. */
#if defined(__has_include)
  #if __has_include(<ueng/llm.h>)
    #define USIDE_HAVE_UENGINE 1
    #include <ueng/llm.h>
  #endif
#endif

/*---------------------------------------------------------------------------
 * umi_translate_text:
 *   Translate 'text' from source_lang -> target_lang using an LLM.
 *   When USIDE_HAVE_UENGINE is defined, this calls into uengine's chat
 *   helper. Returns a newly allocated string that the caller must g_free().
 *   On error, returns NULL and sets *err.
 *
 *   Parameters:
 *     - text:        UTF-8 input text to translate
 *     - source_lang: BCP-47 (e.g. "en"), may be NULL/"" for auto-detect
 *     - target_lang: BCP-47 (e.g. "de"), must not be NULL/empty
 *     - cfg:         Optional UmiLlmCfg with endpoint/keys. If NULL and
 *                    uengine is present, a default config from the env
 *                    will be used.
 *---------------------------------------------------------------------------*/
gchar *umi_translate_text(const gchar *text,
                          const gchar *source_lang,
                          const gchar *target_lang,
#ifdef USIDE_HAVE_UENGINE
                          UmiLlmCfg   *cfg,
#else
                          void        *cfg /* unused without uengine */,
#endif
                          GError     **err) {
  g_return_val_if_fail(text != NULL, NULL);
  g_return_val_if_fail(target_lang != NULL && *target_lang, NULL);

#ifndef USIDE_HAVE_UENGINE
  /* Stub: no LLM integration available. */
  g_set_error(err, g_quark_from_static_string("umi-translate"),
              1, "Translation unavailable: uengine headers not found at build time.");
  return NULL;
#else
  /* Build a minimal system/user prompt pair instructing the model to translate. */
  g_autofree gchar *prompt =
      g_strdup_printf("Translate the following text%s%s to %s. Keep formatting.\n\n%s",
                      (source_lang && *source_lang) ? " from " : "",
                      (source_lang && *source_lang) ? source_lang : "",
                      target_lang, text);

  UmiLlmCfg local = {0};
  if (!cfg) {
    /* Try environment defaults if caller didn't provide a config. */
    umi_llm_cfg_init_from_env(&local);
    cfg = &local;
  }

  /* Issue a single-shot chat call; see uengine docs for details. */
  g_autofree gchar *response = NULL;
  if (!umi_llm_chat_simple(cfg, "system:You are a precise translator.", prompt,
                           &response, err)) {
    return NULL;
  }

  /* Transfer ownership to the caller (duplicate from the auto-free slot). */
  return g_strdup(response);
#endif
}
/*---------------------------------------------------------------------------*/