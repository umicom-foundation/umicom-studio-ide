/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/plugins/transpile/i18n_translate.c
 * PURPOSE:
 *   Build a minimal prompt and call umi_llm_chat_simple(...) to translate text.
 *
 * DESIGN:
 *   - Self-contained: depends only on <glib.h> and public <llm.h>.
 *   - No UI dependencies. Errors reported via (errbuf, errcap).
 *   - Tight, defensive validation + clear comments.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-01
 * License: MIT
 *---------------------------------------------------------------------------*/

#include "i18n_translate.h"
#include <stdarg.h>  /* <-- needed for va_list in set_err */
#include <string.h>  /* strlen (defensive; not strictly required) */

/* Small helper: safe snprintf into errbuf. */
static void
set_err(char *errbuf, gsize errcap, const char *fmt, ...)
{
  if (!errbuf || errcap == 0) return;
  va_list ap;
  va_start(ap, fmt);
  g_vsnprintf(errbuf, errcap, fmt, ap);
  va_end(ap);
}

gchar *
umi_translate_text(const char *input_text,
                   const char *src_lang,
                   const char *dst_lang,
                   const UmiLlmCfg *cfg,
                   char *errbuf,
                   gsize errcap)
{
  if (!cfg)           { set_err(errbuf, errcap, "cfg is NULL"); return NULL; }
  if (!dst_lang || !*dst_lang) {
    set_err(errbuf, errcap, "dst_lang is empty"); return NULL;
  }
  if (!input_text || !*input_text) {
    set_err(errbuf, errcap, "input text is empty"); return NULL;
  }

  /* Build a compact system instruction that’s provider-agnostic. */
  g_autofree gchar *system_prompt = g_strdup_printf(
      "You are a translation engine. Translate strictly from %s to %s. "
      "Keep code blocks and formatting. Only return the translated text.",
      (src_lang && *src_lang) ? src_lang : "auto-detected language",
      dst_lang);

  char *out_text = NULL; /* allocated by backend; caller g_free()s */

  char call_err[512] = {0};

  const gboolean ok = umi_llm_chat_simple(cfg,
                                          system_prompt,           /* system role */
                                          input_text,              /* user text   */
                                          &out_text,               /* OUT: malloc'd */
                                          call_err, sizeof(call_err));
  if (!ok) {
    set_err(errbuf, errcap, "%s", call_err[0] ? call_err : "translation failed");
    return NULL;
  }

  return out_text;
}
/*--- end of file ---*/
