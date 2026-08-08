/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/llm/includes/llm.h
 * PURPOSE: Public LLM API surface (types + function contracts) for the IDE.
 *
 * Design goals:
 *   - Self-contained C header for the LLM module (no cross-tree headers).
 *   - Pure C (C11), minimal dependencies (GLib for basic types/util).
 *   - Stable ABI for app modules while we iterate LLM internals.
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab
 * Date: 2025-10-12
 * License: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LLM_H
#define UMICOM_LLM_H

/* C/GLib basics */
#include <stdbool.h>   /* bool, true, false                                     */
#include <stddef.h>    /* size_t                                                */
#include <glib.h>      /* gchar, guint, gpointer, gsize, GError, GString, etc.  */

/*-----------------------------------------------------------------------------
 * Provider selection
 *  - Keep the enum stable; append new providers at the end.
 *---------------------------------------------------------------------------*/
typedef enum UmiLlmProvider {
  UMI_LLM_PROVIDER_ZAI        = 1,  /* Z.ai (local/remote depending on config)  */
  UMI_LLM_PROVIDER_OPENAI     = 2,  /* OpenAI                                   */
  UMI_LLM_PROVIDER_OLLAMA     = 3,  /* Ollama (local http API)                  */
  UMI_LLM_PROVIDER_LLAMA_CPP  = 4   /* llama.cpp server (ggml/gguf http)        */
} UmiLlmProvider;

/*-----------------------------------------------------------------------------
 * Configuration passed to chat/stream calls
 *  - These fields are what llm_core.c reads today.
 *---------------------------------------------------------------------------*/
typedef struct UmiLlmCfg {
  UmiLlmProvider provider;   /* Which backend to talk to                        */
  bool           stream;     /* true => streaming token callbacks               */

  /* Connection details (optional: NULL => use sensible defaults / env).       */
  const char    *api_base;   /* e.g. "https://api.openai.com"                   */
  const char    *api_path;   /* e.g. "/v1/chat/completions"                     */
  const char    *api_key;    /* secret if required                              */
  const char    *model;      /* model id/name                                   */

  /* Transport / budget */
  int            timeout_ms; /* HTTP timeout or provider call budget in ms      */

  /* (Room for future knobs: temperature, top_p, top_k, etc.)                  */
} UmiLlmCfg;

/* Initialize fields from environment defaults (safe fallbacks).               */
void umi_llm_cfg_init_from_env(UmiLlmCfg *cfg);

/*-----------------------------------------------------------------------------
 * Token alternatives (for entropy/top-k readouts in LLM Lab)
 *---------------------------------------------------------------------------*/
typedef struct UmiLlmTokenAlt {
  char   *token;    /* UTF-8 text for the alternative (caller frees)           */
  double  logprob;  /* natural log probability                                 */
} UmiLlmTokenAlt;

/* Free an array of alts previously returned by the API (safe on NULL).        */
void umi_llm_free_alts(UmiLlmTokenAlt *alts, guint n);

/*-----------------------------------------------------------------------------
 * Streaming callbacks
 *---------------------------------------------------------------------------*/
typedef void (*UmiLlmOnTokenEx)(const gchar            *fragment_utf8,
                                const UmiLlmTokenAlt   *alts,      /* may be NULL */
                                guint                   alts_n,    /* 0 if none  */
                                gpointer                user_data);

typedef void (*UmiLlmOnToken)(const gchar *fragment_utf8, gpointer user_data);

/*-----------------------------------------------------------------------------
 * High-level chat APIs
 *---------------------------------------------------------------------------*/

/* Non-streaming chat. On success, *out_text is g_malloc’d; caller frees.      */
bool umi_llm_chat_simple(const UmiLlmCfg *cfg,
                         const char      *system_prompt_or_null,
                         const char      *user_text,
                         char           **out_text,
                         char            *errbuf,   gsize errcap);

/* Streaming chat delivering fragments with optional token alternatives.       */
bool umi_llm_chat_stream_ex(const UmiLlmCfg *cfg,
                            const char      *system_prompt_or_null,
                            const char      *user_text,
                            UmiLlmOnTokenEx  on_token_ex,
                            gpointer         user_data,
                            char            *errbuf,   gsize errcap);

/* Convenience: streaming without alternatives.                                */
bool umi_llm_chat_stream(const UmiLlmCfg *cfg,
                         const char      *system_prompt_or_null,
                         const char      *user_text,
                         UmiLlmOnToken    on_token,
                         gpointer         user_data,
                         char            *errbuf,   gsize errcap);

/*-----------------------------------------------------------------------------
 * Debug helpers (OpenAI-style SSE line parser)
 *
 * NOTE: Signature matches llm_core.c implementation:
 *   - json_line: one "data: {...}\n" payload line (UTF-8).
 *   - out_event: (optional) set to a newly-allocated event string (e.g. "delta")
 *                Caller must g_free(*out_event) when done. Can be NULL.
 *   - out_alts:  (optional) set to a newly-allocated array of UmiLlmTokenAlt.
 *                Use umi_llm_free_alts() to free. Can be NULL.
 *   - out_n:     (optional) number of alts. Can be NULL.
 * Returns true on successful parse of a meaningful SSE delta, false otherwise.
 *---------------------------------------------------------------------------*/
bool umi_llm_debug_parse_openai_sse_line(const gchar   *json_line,
                                         gchar        **out_event,
                                         UmiLlmTokenAlt **out_alts,
                                         guint         *out_n);

#endif /* UMICOM_LLM_H */
/*--------------------------------- End of file --------------------------------*/
