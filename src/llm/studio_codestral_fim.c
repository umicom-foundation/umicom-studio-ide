/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/studio_codestral_fim.c
 * PURPOSE: Minimal libcurl client for Mistral Codestral FIM endpoint.
 *---------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------------
 * Umicom Studio IDE
 * PURPOSE: Core sources for Umicom Studio IDE.
 * Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
 * Last updated: 2025-10-11
 * ---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

struct membuf { char *p; size_t n; size_t cap; };
static size_t mb_write(void *ptr, size_t sz, size_t nm, void *ud){
  size_t n = sz*nm; struct membuf *b = (struct membuf*)ud;
  if(!b->p){ b->cap = (n? n:1) + 4096; b->p = (char*)malloc(b->cap); b->n=0; }
  if(b->n + n + 1 > b->cap){ size_t nc = (b->cap*2) + n + 1; char *np=(char*)realloc(b->p, nc); if(!np) return 0; b->p=np; b->cap=nc; }
  memcpy(b->p + b->n, ptr, n); b->n += n; b->p[b->n] = 0; return n;
}

static void json_escape(const char *in, char *out, size_t outsz){
  size_t j=0;
  for(size_t i=0; in && in[i] && j+2 < outsz; ++i){
    unsigned char c = (unsigned char)in[i];
    if(c=='\\' || c=='\"'){ if(j+2<outsz){ out[j++]='\\'; out[j++]=c; } }
    else if(c=='\n'){ if(j+2<outsz){ out[j++]='\\'; out[j++]='n'; } }
    else if(c=='\r'){ if(j+2<outsz){ out[j++]='\\'; out[j++]='r'; } }
    else { out[j++]=c; }
  }
  if(outsz) out[j<outsz?j:outsz-1]=0;
}

int studio_codestral_fim(const char *api_key,
                         const char *base_url,
                         const char *model,
                         const char *prefix,
                         const char *suffix,
                         int max_tokens,
                         double temperature,
                         char *out, unsigned long outsz){
  if(!api_key || !*api_key || !prefix || !suffix || !out || outsz==0) return 1;

  CURL *h = curl_easy_init();
  if(!h){ snprintf(out, outsz, "curl init failed"); return 2; }

  char pre[8192], suf[8192];
  json_escape(prefix, pre, sizeof pre);
  json_escape(suffix, suf, sizeof suf);

  char body[20000];
  const char *mdl = (model && *model) ? model : "codestral-latest";
  const char *base = (base_url && *base_url) ? base_url : "https://api.mistral.ai";
  snprintf(body, sizeof body,
    "{"
      "\"model\":\"%s\","
      "\"prompt\":\"%s\","
      "\"suffix\":\"%s\","
      "\"max_tokens\":%d,"
      "\"temperature\":%.3f"
    "}",
    mdl, pre, suf, (max_tokens>0?max_tokens:256), temperature);

  char url[512];
  snprintf(url, sizeof url, "%s/v1/fim/completions", base);

  struct curl_slist *hdr = NULL;
  char auth[512]; snprintf(auth, sizeof auth, "Authorization: Bearer %s", api_key);
  hdr = curl_slist_append(hdr, "Content-Type: application/json");
  hdr = curl_slist_append(hdr, auth);

  struct membuf mb = {0};
  curl_easy_setopt(h, CURLOPT_URL, url);
  curl_easy_setopt(h, CURLOPT_HTTPHEADER, hdr);
  curl_easy_setopt(h, CURLOPT_POSTFIELDS, body);
  curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, mb_write);
  curl_easy_setopt(h, CURLOPT_WRITEDATA, &mb);

  CURLcode rc = curl_easy_perform(h);
  long code = 0; curl_easy_getinfo(h, CURLINFO_RESPONSE_CODE, &code);
  curl_slist_free_all(hdr);
  curl_easy_cleanup(h);

  if(rc != CURLE_OK || code/100 != 2){
    snprintf(out, outsz, "HTTP %ld (curl rc=%d)", code, (int)rc);
    if(mb.p){ free(mb.p); }
    return 3;
  }

  /* Return raw JSON to caller (editor can parse "choices[0].text"/etc depending on spec) */
  snprintf(out, outsz, "%s", mb.p ? mb.p : "");
  free(mb.p);
  return 0;
}
