/* ADDITIVE SHIM:
 * Ensure backend identifiers are visible even if older headers are present.
 * It is safe to include multiple times thanks to the include guard.
 */
#include "ueng/llm.h"
#ifndef UENG_LLM_BACKEND_LLAMA
#define UENG_LLM_BACKEND_LLAMA 1
#endif
#ifndef UENG_LLM_BACKEND_OPENAI
#define UENG_LLM_BACKEND_OPENAI 2
#endif
#ifndef UENG_LLM_BACKEND_OLLAMA
#define UENG_LLM_BACKEND_OLLAMA 3
#endif

/*-----------------------------------------------------------------------------
 * Umicom AuthorEngine AI (uaengine)
 * File: src/llm_llama.c
 * PURPOSE: Thin wrapper around llama.cpp for in-process inference.
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 24-09-2025
 * License: MIT
 *---------------------------------------------------------------------------*/
#include "ueng/llm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --------------------------------------------------------------------------
   Build-time switches controlled by CMake:
     UENG_WITH_LLAMA_EMBED  - enable real llama.cpp integration
     HAVE_LLAMA_H           - defined when <llama.h> is discoverable
   When either is absent, we compile portable NOOP stubs that explain the
   situation at runtime (helpful for contributors without the backend).
---------------------------------------------------------------------------- */

struct ueng_llm_ctx
{
  int placeholder; /* unused in stub; real backend holds llama_* handles */
};

#if defined(UENG_WITH_LLAMA_EMBED) && defined(HAVE_LLAMA_H)
/* -------------------------- REAL IMPLEMENTATION ---------------------------
   NOTE: We intentionally keep this minimal and readable. llama.cpp evolves,
   so this code aims to use only the stable C API (llama.h). If the upstream
   API shifts, this file is the only place we must adjust.
--------------------------------------------------------------------------- */
#include <llama.h>

struct ueng_llm_ctx_real
{
  struct llama_model *model;
  struct llama_context *ctx;
  int n_ctx;
};

ueng_llm_ctx *ueng_llm_open(const char *model_path, int ctx_tokens, char *err, size_t errsz)
{
  if (!model_path || !*model_path)
  {
    if (err && errsz)
      snprintf(err, errsz, "model path is empty");
    return NULL;
  }
  if (ctx_tokens <= 0)
    ctx_tokens = 4096;

  llama_backend_init();
  llama_numa_init(llama_numa_strategy::LLAMA_NUMA_STRATEGY_DISABLED);

  struct llama_model_params mp = llama_model_default_params();
  struct llama_context_params cp = llama_context_default_params();
  cp.n_ctx = ctx_tokens;
  cp.seed = 0;

  struct llama_model *model = llama_load_model_from_file(model_path, mp);
  if (!model)
  {
    if (err && errsz)
      snprintf(err, errsz, "failed to load model: %s", model_path);
    return NULL;
  }
  struct llama_context *lctx = llama_new_context_with_model(model, cp);
  if (!lctx)
  {
    if (err && errsz)
      snprintf(err, errsz, "failed to create llama context");
    llama_free_model(model);
    return NULL;
  }

  struct ueng_llm_ctx_real *R = (struct ueng_llm_ctx_real *)calloc(1, sizeof(*R));
  R->model = model;
  R->ctx = lctx;
  R->n_ctx = ctx_tokens;
  return (ueng_llm_ctx *)R;
}

int ueng_llm_prompt(ueng_llm_ctx *handle, const char *prompt, char *out, size_t outsz)
{
  if (!handle || !prompt || !out || outsz == 0)
    return -1;
  struct ueng_llm_ctx_real *R = (struct ueng_llm_ctx_real *)handle;

  /* Very small prompt-&-complete loop using llama.cpp helpers */
  struct llama_batch batch = llama_batch_init(512, 0, 1);

  int n_tokens = llama_tokenize(R->ctx, prompt, strlen(prompt), batch.tokens, 512, true, true);
  if (n_tokens <= 0)
  {
    llama_batch_free(batch);
    return -2;
  }

  batch.n_tokens = n_tokens;
  if (llama_decode(R->ctx, batch) != 0)
  {
    llama_batch_free(batch);
    return -3;
  }

  /* Greedy sample a few tokens for demonstration */
  size_t used = 0;
  for (int t = 0; t < 64 && used + 8 < outsz; ++t)
  {
    llama_token tok = llama_sample_token_greedy(R->ctx, NULL);
    if (tok == llama_token_eos(R->ctx))
      break;
    const char *piece = llama_token_to_piece(R->ctx, tok);
    size_t L = piece ? strlen(piece) : 0;
    if (L > 0)
    {
      if (used + L >= outsz)
        L = outsz - used - 1;
      memcpy(out + used, piece, L);
      used += L;
      out[used] = '\0';
    }
    /* feed the token back */
    batch.n_tokens = 1;
    batch.tokens[0] = tok;
    batch.pos[0] = n_tokens + t;
    if (llama_decode(R->ctx, batch) != 0)
      break;
  }
  llama_batch_free(batch);
  return 0;
}

void ueng_llm_close(ueng_llm_ctx *handle)
{
  if (!handle)
    return;
  struct ueng_llm_ctx_real *R = (struct ueng_llm_ctx_real *)handle;
  llama_free(R->ctx);
  llama_free_model(R->model);
  free(R);
  llama_backend_free();
}

#else /* STUB (backend not compiled) --------------------------------------- */

struct ueng_llm_ctx *ueng_llm_open(const char *model_path, int ctx_tokens, char *err, size_t errsz)
{
  (void)model_path;
  (void)ctx_tokens;
  if (err && errsz)
  {
    snprintf(err, errsz,
             "llama.cpp backend not built. Reconfigure with -DUENG_WITH_LLAMA_EMBED=ON "
             "and vendor third_party/llama.cpp (providing <llama.h>).");
  }
  return NULL;
}

int ueng_llm_prompt(struct ueng_llm_ctx *ctx, const char *prompt, char *out, size_t outsz)
{
  (void)ctx;
  (void)prompt;
  if (out && outsz)
  {
    out[0] = '\0';
  }
  return -1;
}

void ueng_llm_close(struct ueng_llm_ctx *ctx) { (void)ctx; }

#endif /* UENG_WITH_LLAMA_EMBED && HAVE_LLAMA_H */
