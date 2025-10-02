/*-----------------------------------------------------------------------------
 * Umicom AuthorEngine AI (uaengine)
 * File: src/ueng_config.c
 * PURPOSE: Implementation of a tiny, dependency-free configuration loader.
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 25-09-2025
 * License: MIT
 *---------------------------------------------------------------------------*
 * DESIGN NOTES
 * ------------
 * - We intentionally parse a *flat* subset of YAML: "key: value" pairs only.
 * - Comments start with '#' and are ignored (tolerated anywhere on a line).
 * - Unknown keys are ignored on purpose to allow forward evolution.
 * - This module is *loosely coupled*: it does not include other project
 *   headers and can be compiled as-is in any C17 toolchain.
 * - To keep the rest of the code unchanged, we also provide a helper
 *   to export a few environment variables that existing code already
 *   reads (UENG_LLM_PROVIDER, UENG_LLM_MODEL, etc.).
 *---------------------------------------------------------------------------*/
#include "ueng/config.h"

#include <stdio.h>  /* FILE, fopen, fgets */
#include <stdlib.h> /* getenv, strtol */
#include <string.h> /* strlen, strncpy, strcmp, strchr, memset */

#ifndef _WIN32
#include <ctype.h> /* isspace */
#endif

/*------------------------------ tiny helpers --------------------------------*/

/* trim_in_place: remove leading and trailing ASCII spaces and tabs.
 * This is sufficient for simple config files; Unicode whitespace is not needed. */
static void trim_in_place(char *s)
{
  if (!s)
    return;
  size_t n = strlen(s);
  size_t start = 0, end = n;
  while (start < n && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r' || s[start] == '\n'))
    start++;
  while (end > start &&
         (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\r' || s[end - 1] == '\n'))
    end--;
  if (start > 0)
    memmove(s, s + start, end - start);
  s[end - start] = '\0';
}

/* chomp_comment: cut '#' and everything after it (simple, not inside quotes). */
static void chomp_comment(char *s)
{
  if (!s)
    return;
  char *hash = strchr(s, '#');
  if (hash)
    *hash = '\0';
}

/* copy_str: safe-copy helper that always NUL-terminates. */
static void copy_str(char *dst, size_t dstsz, const char *src)
{
  if (!dst || dstsz == 0)
    return;
  if (!src)
  {
    dst[0] = '\0';
    return;
  }
  strncpy(dst, src, dstsz - 1);
  dst[dstsz - 1] = '\0';
}

/* set_env_if: export environment variable if missing OR if overwrite!=0 */
static void set_env_if(const char *name, const char *value, int overwrite)
{
  if (!name || !value || !*value)
    return;
#ifdef _WIN32
  /* On Windows use _putenv_s which sets (and overwrites) by default.
   * We emulate "skip if not overwrite" by checking GetEnvironmentVariable. */
  char buf[1];
  size_t need = 0;
  errno_t err = 0;
  /* Try to read environment (via getenv) */
  const char *existing = getenv(name);
  if (existing && *existing && !overwrite)
    return;
  /* Set it */
  _putenv_s(name, value);
#else
  const char *existing = getenv(name);
  if (existing && *existing && !overwrite)
    return;
  setenv(name, value, 1 /* overwrite always true here, we guarded above */);
#endif
}

/*------------------------------ API impl ------------------------------------*/

void ueng_config_defaults(UengConfig *c)
{
  if (!c)
    return;
  memset(c, 0, sizeof(*c));

  /* Sensible defaults that make the CLI friendly. */
  copy_str(c->llm_provider, sizeof(c->llm_provider), "ollama");
  copy_str(c->llm_model, sizeof(c->llm_model), "qwen2.5:3b");

  c->serve_port = 8080;
  copy_str(c->serve_host, sizeof(c->serve_host), "127.0.0.1");
  copy_str(c->workspace_dir, sizeof(c->workspace_dir), "workspace");
  copy_str(c->site_root, sizeof(c->site_root), "site");
  /* Optional knobs left empty by default (env or file can fill them): */
  c->openai_api_key[0] = '\0';
  c->openai_base_url[0] = '\0';
  c->ollama_host[0] = '\0'; /* default Ollama host can be inferred by the backend if empty */
  c->llama_model_path[0] = '\0';
}

static void apply_kv(UengConfig *c, const char *key, const char *value)
{
  if (!c || !key || !value)
    return;

  /* NOTE: keys are case-sensitive on purpose to keep it simple. */
  if (strcmp(key, "llm.provider") == 0)
  {
    copy_str(c->llm_provider, sizeof(c->llm_provider), value);
  }
  else if (strcmp(key, "llm.model") == 0)
  {
    copy_str(c->llm_model, sizeof(c->llm_model), value);
  }
  else if (strcmp(key, "openai.api_key") == 0)
  {
    copy_str(c->openai_api_key, sizeof(c->openai_api_key), value);
  }
  else if (strcmp(key, "openai.base_url") == 0)
  {
    copy_str(c->openai_base_url, sizeof(c->openai_base_url), value);
  }
  else if (strcmp(key, "ollama.host") == 0)
  {
    copy_str(c->ollama_host, sizeof(c->ollama_host), value);
  }
  else if (strcmp(key, "llama.model_path") == 0)
  {
    copy_str(c->llama_model_path, sizeof(c->llama_model_path), value);
  }
  else if (strcmp(key, "serve.host") == 0)
  {
    copy_str(c->serve_host, sizeof(c->serve_host), value);
  }
  else if (strcmp(key, "serve.port") == 0)
  {
    long v = strtol(value, NULL, 10);
    if (v > 0 && v < 65536)
      c->serve_port = (int)v;
  }
  else if (strcmp(key, "paths.workspace_dir") == 0)
  {
    copy_str(c->workspace_dir, sizeof(c->workspace_dir), value);
  }
  else if (strcmp(key, "paths.site_root") == 0)
  {
    copy_str(c->site_root, sizeof(c->site_root), value);
  }
  /* else: silently ignore unknown keys to allow forward-compatible files */
}

int ueng_config_load_file(UengConfig *c, const char *path)
{
  if (!c || !path || !*path)
    return 1;
  FILE *f = fopen(path, "r");
  if (!f)
    return 2; /* not found or not readable; keep it non-fatal to caller */

  char line[1024];
  while (fgets(line, sizeof(line), f))
  {
    chomp_comment(line);
    trim_in_place(line);
    if (!*line)
      continue; /* empty */

    /* Expect "key: value" */
    char *colon = strchr(line, ':');
    if (!colon)
      continue; /* ignore malformed lines */
    *colon = '\0';
    char *key = line;
    char *val = colon + 1;
    trim_in_place(key);
    trim_in_place(val);

    if (*key && *val)
      apply_kv(c, key, val);
  }
  fclose(f);
  return 0;
}

void ueng_config_apply_env(UengConfig *c)
{
  if (!c)
    return;
  const char *s;

  if ((s = getenv("UENG_LLM_PROVIDER")) && *s)
    copy_str(c->llm_provider, sizeof(c->llm_provider), s);
  if ((s = getenv("UENG_LLM_MODEL")) && *s)
    copy_str(c->llm_model, sizeof(c->llm_model), s);

  if ((s = getenv("OPENAI_API_KEY")) && *s)
    copy_str(c->openai_api_key, sizeof(c->openai_api_key), s);
  if ((s = getenv("UENG_OPENAI_BASE_URL")) && *s)
    copy_str(c->openai_base_url, sizeof(c->openai_base_url), s);

  if ((s = getenv("UENG_OLLAMA_HOST")) && *s)
    copy_str(c->ollama_host, sizeof(c->ollama_host), s);
  if ((s = getenv("UENG_LLAMA_MODEL_PATH")) && *s)
    copy_str(c->llama_model_path, sizeof(c->llama_model_path), s);

  if ((s = getenv("UENG_SERVE_HOST")) && *s)
    copy_str(c->serve_host, sizeof(c->serve_host), s);
  if ((s = getenv("UENG_SERVE_PORT")) && *s)
  {
    long v = strtol(s, NULL, 10);
    if (v > 0 && v < 65536)
      c->serve_port = (int)v;
  }

  if ((s = getenv("UENG_WORKSPACE_DIR")) && *s)
    copy_str(c->workspace_dir, sizeof(c->workspace_dir), s);
  if ((s = getenv("UENG_SITE_ROOT")) && *s)
    copy_str(c->site_root, sizeof(c->site_root), s);
}

void ueng_config_export_env(const UengConfig *c, int overwrite)
{
  if (!c)
    return;
  set_env_if("UENG_LLM_PROVIDER", c->llm_provider, overwrite);
  set_env_if("UENG_LLM_MODEL", c->llm_model, overwrite);
  set_env_if("UENG_OPENAI_BASE_URL", c->openai_base_url, overwrite);
  set_env_if("UENG_OLLAMA_HOST", c->ollama_host, overwrite);
  set_env_if("UENG_LLAMA_MODEL_PATH", c->llama_model_path, overwrite);
  /* We intentionally do not export OPENAI_API_KEY automatically for security;
   * users should set it explicitly via an environment variable or secrets store. */
  char portbuf[16];
  snprintf(portbuf, sizeof(portbuf), "%d", c->serve_port);
  set_env_if("UENG_SERVE_HOST", c->serve_host, overwrite);
  set_env_if("UENG_SERVE_PORT", portbuf, overwrite);
  set_env_if("UENG_WORKSPACE_DIR", c->workspace_dir, overwrite);
  set_env_if("UENG_SITE_ROOT", c->site_root, overwrite);
}

int ueng_config_init_from(const char *file_or_null, UengConfig *out)
{
  if (!out)
    return 1;
  ueng_config_defaults(out);
  if (file_or_null && *file_or_null)
  {
    /* Non-fatal if missing: keep running with defaults + env. */
    (void)ueng_config_load_file(out, file_or_null);
  }
  ueng_config_apply_env(out);
  return 0;
}
