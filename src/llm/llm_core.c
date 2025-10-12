/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/llm/llm_core.c
 * PURPOSE: Minimal LLM core glue (build-safe stubs matching public API)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
/*
 * Notes:
 * - Prior errors were caused by (a) type/signature mismatches vs llm.h,
 *   (b) nested function definitions, and (c) use of Apple "blocks".
 * - This version:
 *     * Uses exactly the types from include/umicom/llm.h (bool, const UmiLlmCfg*).
 *     * Has no nested functions and no blocks.
 *     * Provides simple stubbed behavior so the UI and tests can run.
 *   Replace the stubbed bodies with your real provider logic later.
 */

#include <string.h>
#include <glib.h>
#include <llm.h>

/*-----------------------------------------------------------------------------
 * Config init from environment (stable defaults + light env overrides)
 *---------------------------------------------------------------------------*/
void umi_llm_cfg_init_from_env(UmiLlmCfg *cfg)
{
  if (!cfg) return;
  memset(cfg, 0, sizeof *cfg);

  /* Sane defaults so the app can start without extra checks. */
  cfg->provider   = UMI_LLM_PROVIDER_OPENAI;
  cfg->timeout_ms = 30000;      /* 30s */
  cfg->stream     = TRUE;

  const gchar *m = g_getenv("UMI_LLM_MODEL");
  if (m && *m) cfg->model = g_strdup(m);

  const gchar *base = g_getenv("UMI_LLM_API_BASE");
  if (base && *base) cfg->api_base = g_strdup(base);

  const gchar *path = g_getenv("UMI_LLM_API_PATH");
  if (path && *path) cfg->api_path = g_strdup(path);

  const gchar *key = g_getenv("UMI_LLM_API_KEY");
  if (key && *key) cfg->api_key = g_strdup(key);

  const gchar *prov = g_getenv("UMI_LLM_PROVIDER");
  if (prov && *prov) {
    if (g_ascii_strcasecmp(prov, "openai") == 0)        cfg->provider = UMI_LLM_PROVIDER_OPENAI;
    else if (g_ascii_strcasecmp(prov, "zai") == 0)      cfg->provider = UMI_LLM_PROVIDER_ZAI;
    else if (g_ascii_strcasecmp(prov, "ollama") == 0)   cfg->provider = UMI_LLM_PROVIDER_OLLAMA;
    else if (g_ascii_strcasecmp(prov, "llama.cpp") == 0 ||
             g_ascii_strcasecmp(prov, "llamacpp") == 0) cfg->provider = UMI_LLM_PROVIDER_LLAMA_CPP;
  }
}

/*-----------------------------------------------------------------------------
 * One-shot (non-streaming) — stub returns a friendly canned reply.
 *---------------------------------------------------------------------------*/
bool umi_llm_chat_simple(const UmiLlmCfg *cfg,
                         const gchar     *system_prompt_or_null,
                         const gchar     *user_text,
                         gchar          **out_text,
                         gchar           *errbuf,
                         gsize            errcap)
{
  (void)cfg; (void)system_prompt_or_null;

  if (out_text) *out_text = NULL;
  if (!user_text || !*user_text) {
    if (errbuf && errcap) g_strlcpy(errbuf, "empty user message", errcap);
    return false;
  }
  if (!out_text) {
    if (errbuf && errcap) g_strlcpy(errbuf, "out_text is NULL", errcap);
    return false;
  }

  *out_text = g_strdup("🤖 (stub) LLM backend not wired yet. "
                       "Replace llm_core.c with a real provider implementation.");
  if (errbuf && errcap) errbuf[0] = '\0';
  return true;
}

/*-----------------------------------------------------------------------------
 * Streaming (extended) — stub emits a few fragments through the callback.
 * Signature matches include/umicom/llm.h exactly.
 *---------------------------------------------------------------------------*/
bool umi_llm_chat_stream_ex(const UmiLlmCfg *cfg,
                            const gchar *system_prompt_or_null,
                            const gchar *user_text,
                            UmiLlmOnTokenEx on_token_ex,
                            gpointer on_token_ud,
                            gchar *errbuf, gsize errcap)
{
  (void)cfg;

  if (!user_text || !*user_text) {
    if (errbuf && errcap) g_strlcpy(errbuf, "empty user message", errcap);
    return false;
  }
  if (!on_token_ex) {
    if (errbuf && errcap) g_strlcpy(errbuf, "no stream callback provided", errcap);
    return false;
  }

  on_token_ex("🤖 (stub) ", NULL, 0, on_token_ud);
  if (system_prompt_or_null && *system_prompt_or_null)
    on_token_ex("[sys prompt active] ", NULL, 0, on_token_ud);
  on_token_ex("Streaming placeholder. ", NULL, 0, on_token_ud);
  on_token_ex("Swap this with a real provider.\n", NULL, 0, on_token_ud);

  if (errbuf && errcap) errbuf[0] = '\0';
  return true;
}

/*-----------------------------------------------------------------------------
 * Back-compat streaming wrapper — adapts simple callback to extended form.
 * (All at file scope; no nested functions; no blocks.)
 *---------------------------------------------------------------------------*/
struct _UmiBridgeCtx {
  void (*cb)(const gchar *fragment, gpointer user_data);
  gpointer ud;
};
static void _umi_ex_trampoline(const gchar *frag,
                               const UmiLlmTokenAlt *alts,
                               guint alts_n,
                               gpointer user)
{
  (void)alts; (void)alts_n;
  struct _UmiBridgeCtx *b = (struct _UmiBridgeCtx*)user;
  if (b && b->cb && frag) b->cb(frag, b->ud);
}

bool umi_llm_chat_stream(const UmiLlmCfg *cfg,
                         const gchar     *system_prompt_or_null,
                         const gchar     *user_text,
                         void (*on_token)(const gchar *fragment, gpointer user_data),
                         gpointer         on_token_ud,
                         gchar           *errbuf,
                         gsize            errcap)
{
  struct _UmiBridgeCtx b = { on_token, on_token_ud };
  return umi_llm_chat_stream_ex(cfg, system_prompt_or_null, user_text,
                                _umi_ex_trampoline, &b, errbuf, errcap);
}

/*-----------------------------------------------------------------------------
 * Debug/test helper — minimal SSE JSON passthrough for now.
 *---------------------------------------------------------------------------*/
bool umi_llm_debug_parse_openai_sse_line(const gchar *json_line,
                                         gchar      **out_fragment,
                                         UmiLlmTokenAlt **out_alts,
                                         guint       *out_alts_n)
{
  if (out_fragment) *out_fragment = NULL;
  if (out_alts)     *out_alts = NULL;
  if (out_alts_n)   *out_alts_n = 0;

  if (!json_line || !*json_line) return false;

  /* Passthrough stub — replace with real JSON parsing later. */
  if (out_fragment) *out_fragment = g_strdup(json_line);
  return true;
}

/*-----------------------------------------------------------------------------
 * Helper to free alts list.
 *---------------------------------------------------------------------------*/
void umi_llm_free_alts(UmiLlmTokenAlt *alts, guint n)
{
  if (!alts) return;
  for (guint i = 0; i < n; i++) g_free(alts[i].token);
  g_free(alts);
}

/*--------------------------------- End of file --------------------------------*/
