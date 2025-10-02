/*---------------------------------------------------------------------------
* Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
*---------------------------------------------------------------------------*/
#include "umicom/i18n.h"
#include "umicom/llm.h"
static const char *SYS = "You are a professional translator. Return only the translated text.";
bool umi_translate_text(const gchar *input,
                        const UmiI18nOpts *opts,
                        gchar **out_text,
                        gchar *errbuf, gsize errcap){
  if(!input||!*input||!opts||!opts->dst_locale){
    g_strlcpy(errbuf,"invalid arguments", errcap); return false;
  }
  UmiLlmCfg cfg; umi_llm_cfg_init_from_env(&cfg); cfg.stream = FALSE;
  gchar *user = g_strdup_printf("Translate to %s (formal=%s):\n%s",
    opts->dst_locale, opts->formal? "true":"false", input);
  bool ok = umi_llm_chat_simple(&cfg, SYS, user, out_text, errbuf, errcap);
  g_free(user);
  return ok;
}
