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

/*-----------------------------------------------------------------------------
 * Configuration: default token triplet used by many FIM-capable models.
 * You can override via environment variables if needed:
 *   UMI_FIM_PRE, UMI_FIM_MID, UMI_FIM_SUF
 *---------------------------------------------------------------------------*/
static const char *default_pre(void) { return g_getenv("UMI_FIM_PRE") ?: "<PRE>"; }
static const char *default_mid(void) { return g_getenv("UMI_FIM_MID") ?: "<MID>"; }
static const char *default_suf(void) { return g_getenv("UMI_FIM_SUF") ?: "<SUF>"; }

/* UTF-8–safe substring using byte offsets, clamping split to character boundary. */
static inline gsize
utf8_clamp_offset(const char *s, gsize want)
{
  if (!s) return 0;
  const char *start = s;
  const char *end   = s + want;
  /* If we land in the middle of a codepoint, walk forward to next boundary. */
  while (end > start && (*end & 0xC0) == 0x80) end++;
  return (gsize)(end - start);
}

/**
 * umi_fim_split_buffer:
 * @buf:     Input UTF-8 buffer to split
 * @cursor:  Desired byte offset for split (caret position)
 * @out_pre: (out) Newly-allocated prefix text (owned by caller)
 * @out_suf: (out) Newly-allocated suffix text (owned by caller)
 *
 * Splits @buf into two parts around @cursor, ensuring we don't cut through a
 * UTF-8 codepoint. Both outputs are always set (empty strings when appropriate).
 *
 * Returns: %TRUE on success.
 */
gboolean
umi_fim_split_buffer(const char *buf,
                     gsize       cursor,
                     char      **out_pre,
                     char      **out_suf)
{
  g_return_val_if_fail(out_pre != NULL && out_suf != NULL, FALSE);

  if (!buf) buf = "";
  const gsize len   = strlen(buf);
  const gsize split = utf8_clamp_offset(buf, cursor > len ? len : cursor);

  *out_pre = g_strndup(buf, split);
  *out_suf = g_strdup(buf + split);
  return TRUE;
}

/**
 * umi_fim_build_prompt:
 * @pre:    Prefix text (before the "hole")
 * @suf:    Suffix text (after the "hole")
 * @pre_t:  (nullable) Token to mark start of prefix (defaults to env/constant)
 * @mid_t:  (nullable) Token to mark the "hole"/middle
 * @suf_t:  (nullable) Token to mark start of suffix
 *
 * Constructs a single composite prompt suitable for FIM-aware models:
 *   <PRE> <pre> <MID> <suf> <SUF>
 * The exact tokens are configurable and may vary per provider/model.
 *
 * Returns: Newly allocated string to feed to the model.
 */
char *
umi_fim_build_prompt(const char *pre,
                     const char *suf,
                     const char *pre_t,
                     const char *mid_t,
                     const char *suf_t)
{
  if (!pre) pre = "";
  if (!suf) suf = "";

  const char *P = pre_t ?: default_pre();
  const char *M = mid_t ?: default_mid();
  const char *S = suf_t ?: default_suf();

  /* Precompute total size to avoid realloc churn. */
  gsize total = strlen(P) + strlen(pre) + strlen(M) + strlen(suf) + strlen(S) + 1;
  char *out = g_malloc0(total);
  /* Concatenate in strict order expected by FIM decoders. */
  g_strlcpy(out, P, total);
  g_strlcat(out, pre, total);
  g_strlcat(out, M, total);
  g_strlcat(out, suf, total);
  g_strlcat(out, S, total);
  return out;
}

/**
 * umi_fim_build_prompt_from_buffer:
 * Convenience wrapper: split @buf at @cursor, then build a FIM prompt.
 * See umi_fim_split_buffer() and umi_fim_build_prompt() for details.
 */
char *
umi_fim_build_prompt_from_buffer(const char *buf,
                                 gsize       cursor,
                                 const char *pre_t,
                                 const char *mid_t,
                                 const char *suf_t)
{
  char *pre = NULL, *suf = NULL;
  umi_fim_split_buffer(buf, cursor, &pre, &suf);
  char *out = umi_fim_build_prompt(pre, suf, pre_t, mid_t, suf_t);
  g_free(pre);
  g_free(suf);
  return out;
}
/*---------------------------------------------------------------------------*/
