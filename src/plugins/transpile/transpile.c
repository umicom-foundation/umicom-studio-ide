/*---------------------------------------------------------------------------
* Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
*---------------------------------------------------------------------------*/
#include "umicom/transpile.h"
#include "umicom/llm.h"
#include <string.h>
static const char *SYS =
  "You are a precise code translator. Preserve semantics and produce idiomatic target code.";
bool umi_transpile_code(const gchar *source,
                        const UmiTranspileOpts *opts,
                        gchar **out_text,
                        gchar *errbuf, gsize errcap){
  if(!source||!*source||!opts||!opts->src_lang||!opts->dst_lang){
    g_strlcpy(errbuf,"invalid arguments", errcap); return false;
  }
  UmiLlmCfg cfg; umi_llm_cfg_init_from_env(&cfg); cfg.stream = FALSE;
  gchar *user = g_strdup_printf(
    "Translate from %s to %s. Keep comments=%s. Explain=%s.\nSOURCE:\n```\n%s\n```",
    opts->src_lang, opts->dst_lang,
    opts->keep_comments? "yes":"no",
    opts->add_explanations? "yes":"no",
    source);
  bool ok = umi_llm_chat_simple(&cfg, SYS, user, out_text, errbuf, errcap);
  g_free(user);
  return ok;
}
