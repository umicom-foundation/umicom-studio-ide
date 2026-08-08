/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/llm/studio_codestral_fim.c
 * PURPOSE: Utilities for "Fill-In-the-Middle" (FIM) prompt shaping used by
 *          code models (e.g., Codestral).  Provides helpers to:
 *            - split a buffer at caret/cursor
 *            - build a FIM composite prompt with model-specific tokens
 *            - (optionally) strip placeholder marker near the split
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>
#include <string.h>
#include <llm.h>

/* Small helpers read environment overrides; fall back to defaults. */
static const char *default_pre(void) {
  const char *v = g_getenv("UMI_FIM_PRE");
  return (v && *v) ? v : "<PRE>";
}
static const char *default_mid(void) {
  const char *v = g_getenv("UMI_FIM_MID");
  return (v && *v) ? v : "<MID>";
}
static const char *default_suf(void) {
  const char *v = g_getenv("UMI_FIM_SUF");
  return (v && *v) ? v : "<SUF>";
}

/* Build a FIM prompt: [pre][prompt][mid][suffix][suf] */
gchar *umi_fim_build_prompt(const char *prompt,
                            const char *suffix,
                            const char *pre_t,  /* optional override token */
                            const char *mid_t,  /* optional override token */
                            const char *suf_t)  /* optional override token */
{
  /* Resolve tokens with portable ternary, not GNU ':?'. */
  const char *P = (pre_t && *pre_t) ? pre_t : default_pre();
  const char *M = (mid_t && *mid_t) ? mid_t : default_mid();
  const char *S = (suf_t && *suf_t) ? suf_t : default_suf();

  const char *A = prompt ? prompt : "";
  const char *B = suffix ? suffix : "";

  /* Precompute length to allocate once. */
  gsize len = strlen(P) + strlen(A) + strlen(M) + strlen(B) + strlen(S);
  gchar *out = g_malloc(len + 1);

  /* Concatenate in order. */
  out[0] = '\0';
  strcat(out, P);
  strcat(out, A);
  strcat(out, M);
  strcat(out, B);
  strcat(out, S);

  return out; /* caller frees with g_free() */
}
