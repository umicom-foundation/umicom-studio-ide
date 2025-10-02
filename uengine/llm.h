/*-----------------------------------------------------------------------------
 * Umicom AuthorEngine AI (uaengine)
 * File: include/ueng/llm.h
 * PURPOSE: Minimal cross-backend LLM API (OpenAI / Ollama / llama.cpp)
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 24-09-2025
 * License: MIT
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
 * Module notes (added, non-destructive):
 * MODULE OVERVIEW
 * ----------------
 * Purpose: Stable, tiny abstraction for pluggable LLM backends (OpenAI, Ollama, llama.cpp).
 * Responsibilities:
 *   - Provide an opaque session handle (ueng_llm_ctx) to the rest of the program.
 *   - Offer the simplest possible API to send a prompt and get a short completion.
 *   - Keep the main binary linkable even when no backend is compiled in (open returns NULL +
 error).

 * Extending:
 *   - Add new providers by implementing the three functions in a new llm_*.c translation unit.
 *   - Keep this header vendor-neutral; avoid leaking provider-specific types here.
 *---------------------------------------------------------------------------*/

#ifndef UENG_LLM_H
#define UENG_LLM_H

#include <stddef.h> /* size_t */

#ifdef __cplusplus
extern "C"
{
#endif

  /*-----------------------------------------------------------------------------
   * Backend identifiers
   *
   * These constants are used in switch/if statements across the backend
   * implementations. On some systems the previous header set did not define
   * them, which caused "undeclared identifier" build errors in llm_llama.c.
   * Defining them here keeps all translation units in sync.
   *---------------------------------------------------------------------------*/
  typedef enum ueng_llm_backend_e
  {
    UENG_LLM_BACKEND_LLAMA = 1,  /* local: built-in llama.cpp */
    UENG_LLM_BACKEND_OPENAI = 2, /* cloud: OpenAI REST API    */
    UENG_LLM_BACKEND_OLLAMA = 3  /* local: Ollama HTTP API    */
  } ueng_llm_backend_t;

  /* Opaque handle for an in-process LLM session. Concrete definition lives in
   * the backend .c (llm_llama.c when llama.cpp is enabled). */
  typedef struct ueng_llm_ctx ueng_llm_ctx;

  /* Create a new LLM context.
   * - model_path: for local backends (llama.cpp); for remote backends you can
   *               pass a model name (e.g., "gpt-4o-mini" or "llama3:8b").
   * - ctx_tokens: desired context size; backends may clamp it.
   * - err/errsz : optional human-readable error buffer.
   * Returns non-NULL on success, NULL on failure (with 'err' filled when given).
   */
  ueng_llm_ctx *ueng_llm_open(const char *model_path, int ctx_tokens, char *err, size_t errsz);

  /* Generate a short completion for 'prompt' into 'out' (NUL-terminated). */
  int ueng_llm_prompt(ueng_llm_ctx *ctx, const char *prompt, char *out, size_t outsz);

  /* Destroy a context (safe to call with NULL). */
  void ueng_llm_close(ueng_llm_ctx *ctx);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UENG_LLM_H */
