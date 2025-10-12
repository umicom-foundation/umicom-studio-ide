/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/llm/llm_http.c
 * PURPOSE: HTTP helpers (libcurl) + SSE-ish streaming with privacy guard
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-11 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>                /* Core GLib types and utilities            */
#include <curl/curl.h>          /* libcurl for HTTP requests                 */
#include <string.h>             /* memcpy/strncmp/strlen, etc.               */
#include <stdlib.h>             /* getenv, strtol                            */

#include <privacy.h>     /* umi_privacy_allow_url()                   */

/*------------------------------------------------------------------------------
 * Small accumulator used by libcurl to gather response bytes.
 *----------------------------------------------------------------------------*/
typedef struct {
  GString *buf;                 /* Dynamically-grown buffer for response body */
} UmiCurlAcc;

/* libcurl write callback: append received chunk into our GString accumulator. */
static size_t on_write(char *ptr, size_t size, size_t nmemb, void *userdata)
{
  /* Total bytes in this chunk = size * nmemb (libcurl contract). */
  size_t n = size * nmemb;

  /* Cast userdata back to our accumulator. */
  UmiCurlAcc *acc = (UmiCurlAcc*)userdata;

  /* Append the chunk as raw bytes (binary-safe). */
  g_string_append_len(acc->buf, ptr, n);

  /* Return number of bytes “taken”; libcurl requires exact consumption count. */
  return n;
}

/*------------------------------------------------------------------------------
 * Utility: add a header line to curl_slist (helper to keep callsites tidy).
 *----------------------------------------------------------------------------*/
static void add_header(struct curl_slist **hdrs, const char *line)
{
  if (!line || !*line) return;        /* Ignore empty input safely.          */
  *hdrs = curl_slist_append(*hdrs, line);
}

/*------------------------------------------------------------------------------
 * Utility: add "Authorization: Bearer <token>" if token is supplied.
 *----------------------------------------------------------------------------*/
static void add_bearer(struct curl_slist **hdrs, const char *token)
{
  if (token && *token) {
    /* Build header line with token; freed after appended to list.            */
    gchar *h = g_strdup_printf("Authorization: Bearer %s", token);
    *hdrs = curl_slist_append(*hdrs, h);
    g_free(h);
  }
}

/*------------------------------------------------------------------------------
 * GET JSON - convenience wrapper.
 * - url: endpoint to call (privacy-checked)
 * - bearer_token_or_null: optional “Bearer <token>” if provided
 * - timeout_ms: request timeout in milliseconds
 * - out_body: receives malloc'd UTF-8 body (g_free by caller) on success
 * - errbuf/errcap: human-readable error text on failure
 * Returns TRUE on 2xx + CURLE_OK; FALSE otherwise.
 *----------------------------------------------------------------------------*/
gboolean umi_http_get_json(const gchar *url,
                           const gchar *bearer_token_or_null,
                           guint        timeout_ms,
                           gchar      **out_body,
                           gchar       *errbuf,
                           gsize        errcap)
{
  /* Initialize outputs to a safe state. */
  if (out_body) *out_body = NULL;

  /* Respect privacy mode (“local-only”) – block remote hosts when enabled. */
  if (!umi_privacy_allow_url(url, errbuf, (unsigned)errcap))
    return FALSE;

  /* Create easy handle; fail early if libcurl is unavailable. */
  CURL *C = curl_easy_init();
  if (!C) { g_strlcpy(errbuf, "curl init failed", errcap); return FALSE; }

  /* Headers: JSON content type and optional Authorization. */
  struct curl_slist *hdr = NULL;
  add_header(&hdr, "Accept: application/json");
  add_bearer(&hdr, bearer_token_or_null);

  /* Accumulator for response body. */
  UmiCurlAcc acc = { .buf = g_string_new(NULL) };

  /* Core options: URL, headers, timeout, and write callback. */
  curl_easy_setopt(C, CURLOPT_URL, url);
  curl_easy_setopt(C, CURLOPT_HTTPHEADER, hdr);
  curl_easy_setopt(C, CURLOPT_TIMEOUT_MS, (long)timeout_ms);
  curl_easy_setopt(C, CURLOPT_WRITEFUNCTION, on_write);
  curl_easy_setopt(C, CURLOPT_WRITEDATA, &acc);

  /* Execute request. */
  CURLcode rc = curl_easy_perform(C);

  /* Capture HTTP response code (0 if transfer-level error). */
  long code = 0; curl_easy_getinfo(C, CURLINFO_RESPONSE_CODE, &code);

  /* Success = transport OK and 2xx status. */
  gboolean ok = (rc == CURLE_OK && code >= 200 && code < 300);

  if (!ok) {
    /* Provide readable context: HTTP code plus curl error text. */
    g_snprintf(errbuf, errcap, "HTTP %ld: %s", code, curl_easy_strerror(rc));
  } else if (out_body) {
    /* Hand off buffer to caller (don’t free here). */
    *out_body = g_string_free(acc.buf, FALSE);
    acc.buf = NULL; /* mark as moved */
  }

  /* Cleanup (safe if NULL). */
  if (acc.buf) g_string_free(acc.buf, TRUE);
  curl_slist_free_all(hdr);
  curl_easy_cleanup(C);
  return ok;
}

/*------------------------------------------------------------------------------
 * POST JSON - convenience wrapper.
 * - url: endpoint to call (privacy-checked)
 * - bearer_token_or_null: optional “Bearer <token>”
 * - json_body: UTF-8 json payload string (may be "")
 * - timeout_ms: request timeout
 * - out_body: receives malloc'd UTF-8 response body
 * - errbuf/errcap: error on failure
 *----------------------------------------------------------------------------*/
gboolean umi_http_post_json(const gchar *url,
                            const gchar *bearer_token_or_null,
                            const gchar *json_body,
                            guint        timeout_ms,
                            gchar      **out_body,
                            gchar       *errbuf,
                            gsize        errcap)
{
  if (out_body) *out_body = NULL;
  if (!umi_privacy_allow_url(url, errbuf, (unsigned)errcap))
    return FALSE;

  CURL *C = curl_easy_init();
  if (!C) { g_strlcpy(errbuf, "curl init failed", errcap); return FALSE; }

  struct curl_slist *hdr = NULL;
  add_header(&hdr, "Content-Type: application/json");
  add_header(&hdr, "Accept: application/json");
  add_bearer(&hdr, bearer_token_or_null);

  UmiCurlAcc acc = { .buf = g_string_new(NULL) };

  curl_easy_setopt(C, CURLOPT_URL, url);
  curl_easy_setopt(C, CURLOPT_HTTPHEADER, hdr);
  curl_easy_setopt(C, CURLOPT_POSTFIELDS, json_body ? json_body : "");
  curl_easy_setopt(C, CURLOPT_TIMEOUT_MS, (long)timeout_ms);
  curl_easy_setopt(C, CURLOPT_WRITEFUNCTION, on_write);
  curl_easy_setopt(C, CURLOPT_WRITEDATA, &acc);

  CURLcode rc = curl_easy_perform(C);
  long code = 0; curl_easy_getinfo(C, CURLINFO_RESPONSE_CODE, &code);
  gboolean ok = (rc == CURLE_OK && code >= 200 && code < 300);

  if (!ok) {
    g_snprintf(errbuf, errcap, "HTTP %ld: %s", code, curl_easy_strerror(rc));
  } else if (out_body) {
    *out_body = g_string_free(acc.buf, FALSE);
    acc.buf = NULL;
  }

  if (acc.buf) g_string_free(acc.buf, TRUE);
  curl_slist_free_all(hdr);
  curl_easy_cleanup(C);
  return ok;
}

/*------------------------------------------------------------------------------
 * Streaming POST (SSE-like): sends a POST body and yields raw chunks to a
 * user callback. This is enough for text streaming APIs (OpenAI, etc.).
 *----------------------------------------------------------------------------*/
typedef void (*UmiHttpOnChunk)(const char *data, gsize len, gpointer user);

typedef struct {
  UmiHttpOnChunk on_chunk;      /* User callback to receive streamed bytes    */
  gpointer       user;          /* Opaque user pointer                        */
} UmiStreamCb;

/* libcurl write callback for streaming mode: forward chunk to user callback. */
static size_t on_stream(char *ptr, size_t size, size_t nmemb, void *userdata)
{
  size_t n = size * nmemb;
  UmiStreamCb *cb = (UmiStreamCb*)userdata;

  /* Only call out if a chunk actually arrived. */
  if (cb && cb->on_chunk && n > 0)
    cb->on_chunk(ptr, n, cb->user);

  return n; /* must report exact bytes consumed */
}

/* Streaming POST helper. */
gboolean umi_http_post_stream(const gchar *url,
                              const gchar *bearer_token_or_null,
                              const gchar *json_body,
                              guint        timeout_ms,
                              UmiHttpOnChunk on_chunk,
                              gpointer     user,
                              gchar       *errbuf,
                              gsize        errcap)
{
  if (!umi_privacy_allow_url(url, errbuf, (unsigned)errcap))
    return FALSE;

  CURL *C = curl_easy_init();
  if (!C) { g_strlcpy(errbuf, "curl init failed", errcap); return FALSE; }

  struct curl_slist *hdr = NULL;
  add_header(&hdr, "Content-Type: application/json");
  add_header(&hdr, "Accept: text/event-stream"); /* many LLMs use SSE-ish */
  add_bearer(&hdr, bearer_token_or_null);

  UmiStreamCb cb = { .on_chunk = on_chunk, .user = user };

  curl_easy_setopt(C, CURLOPT_URL, url);
  curl_easy_setopt(C, CURLOPT_HTTPHEADER, hdr);
  curl_easy_setopt(C, CURLOPT_POSTFIELDS, json_body ? json_body : "");
  curl_easy_setopt(C, CURLOPT_TIMEOUT_MS, (long)timeout_ms);
  curl_easy_setopt(C, CURLOPT_WRITEFUNCTION, on_stream);
  curl_easy_setopt(C, CURLOPT_WRITEDATA, &cb);

  CURLcode rc = curl_easy_perform(C);
  long code = 0; curl_easy_getinfo(C, CURLINFO_RESPONSE_CODE, &code);
  gboolean ok = (rc == CURLE_OK && code >= 200 && code < 300);

  if (!ok)
    g_snprintf(errbuf, errcap, "HTTP %ld: %s", code, curl_easy_strerror(rc));

  curl_slist_free_all(hdr);
  curl_easy_cleanup(C);
  return ok;
}
