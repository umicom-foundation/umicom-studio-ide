/* LLM HTTP helpers (libcurl) + privacy guard */

/* -----------------------------------------------------------------------------
 * Umicom Studio IDE
 * PURPOSE: Core sources for Umicom Studio IDE.
 * Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
 * Last updated: 2025-10-11
 * ---------------------------------------------------------------------------*/
#include <umicom/llm_http.h>
#include <umicom/privacy.h>
#include <curl/curl.h>

typedef struct { GString *buf; } Acc;
static size_t on_write(char *ptr, size_t size, size_t nmemb, void *ud){
  Acc *a = (Acc*)ud; g_string_append_len(a->buf, ptr, size*nmemb); return size*nmemb;
}

bool umi_http_post_json(const gchar *url,
                        const gchar *bearer_token_or_null,
                        const gchar *json_body,
                        guint        timeout_ms,
                        gchar      **out_body,
                        gchar       *errbuf,
                        gsize        errcap){
  *out_body = NULL;
  if(!umi_privacy_allow_url(url, errbuf, (unsigned)errcap)) return false;

  CURL *C = curl_easy_init();
  if(!C){ g_strlcpy(errbuf,"curl init failed",errcap); return false; }
  struct curl_slist *hdr = NULL;
  hdr = curl_slist_append(hdr, "Content-Type: application/json");
  if(bearer_token_or_null && *bearer_token_or_null){
    gchar *auth = g_strdup_printf("Authorization: Bearer %s", bearer_token_or_null);
    hdr = curl_slist_append(hdr, auth); g_free(auth);
  }
  Acc a; a.buf = g_string_new(NULL);
  curl_easy_setopt(C, CURLOPT_URL, url);
  curl_easy_setopt(C, CURLOPT_HTTPHEADER, hdr);
  curl_easy_setopt(C, CURLOPT_POSTFIELDS, json_body?json_body:"");
  curl_easy_setopt(C, CURLOPT_TIMEOUT_MS, (long)timeout_ms);
  curl_easy_setopt(C, CURLOPT_WRITEFUNCTION, on_write);
  curl_easy_setopt(C, CURLOPT_WRITEDATA, &a);

  CURLcode rc = curl_easy_perform(C);
  long code = 0; curl_easy_getinfo(C, CURLINFO_RESPONSE_CODE, &code);
  bool ok = (rc==CURLE_OK && code>=200 && code<300);
  if(!ok){ g_snprintf(errbuf, errcap, "HTTP %ld: %s", code, curl_easy_strerror(rc)); }
  else { *out_body = g_string_free(a.buf, FALSE); a.buf = NULL; }

  if(a.buf) g_string_free(a.buf, TRUE);
  curl_slist_free_all(hdr); curl_easy_cleanup(C);
  return ok;
}

/* Streaming (SSE-like) */
typedef struct { UmiHttpOnChunk on_chunk; gpointer ud; } CbWrap;
static size_t on_stream(char *ptr, size_t size, size_t nmemb, void *userdata){
  CbWrap *cb = (CbWrap*)userdata;
  size_t n = size*nmemb;
  if(cb->on_chunk && n>0) cb->on_chunk(ptr, n, cb->ud);
  return n;
}

bool umi_http_post_stream(const gchar *url,
                          const gchar *bearer_token_or_null,
                          const gchar *json_body,
                          guint        timeout_ms,
                          UmiHttpOnChunk on_chunk,
                          gpointer     ud,
                          gchar       *errbuf,
                          gsize        errcap){
  if(!umi_privacy_allow_url(url, errbuf, (unsigned)errcap)) return false;

  CURL *C = curl_easy_init();
  if(!C){ g_strlcpy(errbuf,"curl init failed",errcap); return false; }
  struct curl_slist *hdr = NULL;
  hdr = curl_slist_append(hdr, "Content-Type: application/json");
  if(bearer_token_or_null && *bearer_token_or_null){
    gchar *auth = g_strdup_printf("Authorization: Bearer %s", bearer_token_or_null);
    hdr = curl_slist_append(hdr, auth); g_free(auth);
  }
  CbWrap cb = { on_chunk, ud };
  curl_easy_setopt(C, CURLOPT_URL, url);
  curl_easy_setopt(C, CURLOPT_HTTPHEADER, hdr);
  curl_easy_setopt(C, CURLOPT_POSTFIELDS, json_body?json_body:"");
  curl_easy_setopt(C, CURLOPT_TIMEOUT_MS, (long)timeout_ms);
  curl_easy_setopt(C, CURLOPT_WRITEFUNCTION, on_stream);
  curl_easy_setopt(C, CURLOPT_WRITEDATA, &cb);

  CURLcode rc = curl_easy_perform(C);
  long code = 0; curl_easy_getinfo(C, CURLINFO_RESPONSE_CODE, &code);
  bool ok = (rc==CURLE_OK && code>=200 && code<300);
  if(!ok){ g_snprintf(errbuf, errcap, "HTTP %ld: %s", code, curl_easy_strerror(rc)); }
  curl_slist_free_all(hdr); curl_easy_cleanup(C);
  return ok;
}
