#ifndef UMICOM_LLM_H
#define UMICOM_LLM_H
/* Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT */

#include <stdbool.h>
#include <glib.h>

typedef enum {
  UMI_LLM_PROVIDER_ZAI = 0,
  UMI_LLM_PROVIDER_OPENAI,
  UMI_LLM_PROVIDER_OLLAMA,
  UMI_LLM_PROVIDER_LLAMA_CPP
} UmiLlmProvider;

typedef struct {
  gchar *api_base;
  gchar *api_path;
  gchar *api_key;
  gchar *model;
  guint  timeout_ms;
  gboolean stream;
  UmiLlmProvider provider;
} UmiLlmCfg;

void umi_llm_cfg_init_from_env(UmiLlmCfg *cfg);

/* Non-streaming one-shot chat. */
bool umi_llm_chat_simple(const UmiLlmCfg *cfg,
                         const gchar     *system_prompt_or_null,
                         const gchar     *user_text,
                         gchar          **out_text,
                         gchar           *errbuf,
                         gsize            errcap);

/* Alternatives structure for Token Inspector. */
typedef struct {
  gchar *token;   /* alternative token text */
  double logprob; /* log probability */
} UmiLlmTokenAlt;

/* Extended streaming callback with optional alternatives/logprobs. */
typedef void (*UmiLlmOnTokenEx)(
  const gchar *fragment,                 /* streamed text delta (may be NULL) */
  const UmiLlmTokenAlt *alts,            /* optional top-k alternatives */
  guint alts_n,                          /* number of alternatives */
  gpointer user_data
);

/* Streaming chat (extended). */
bool umi_llm_chat_stream_ex(const UmiLlmCfg *cfg,
                            const gchar *system_prompt_or_null,
                            const gchar *user_text,
                            UmiLlmOnTokenEx on_token_ex,
                            gpointer on_token_ud,
                            gchar *errbuf, gsize errcap);

/* Back-compat streaming wrapper. */
typedef void (*UmiLlmOnToken)(const gchar *fragment, gpointer user_data);
bool umi_llm_chat_stream(const UmiLlmCfg *cfg,
                         const gchar     *system_prompt_or_null,
                         const gchar     *user_text,
                         UmiLlmOnToken    on_token,
                         gpointer         on_token_ud,
                         gchar           *errbuf,
                         gsize            errcap);

/* Debug/test helper: parse a single OpenAI SSE line payload (JSON after "data: ").
 * Allocates *out_alts on success (use umi_llm_free_alts to free). Returns TRUE if any content.
 */
bool umi_llm_debug_parse_openai_sse_line(const gchar *json_line,
                                         gchar      **out_fragment,
                                         UmiLlmTokenAlt **out_alts,
                                         guint       *out_alts_n);

/* Free an alternatives list allocated by the parser. */
void umi_llm_free_alts(UmiLlmTokenAlt *alts, guint n);

#endif /* UMICOM_LLM_H */
