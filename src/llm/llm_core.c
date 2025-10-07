/* SSE stub (credits preserved above) */
static void umi__parse_sse_buffer(UmiStreamCtx *sc) { (void)sc; }
/* SSE stub (credits preserved above) */

/* LLM Core � OpenAI logprobs top-k + robust SSE parsing + debug helpers */
#include "umicom/llm.h"
#include "umicom/llm_http.h"
#include <json-glib/json-glib.h>
#include <string.h>
#include <math.h>

#define ZAI_BASE  "https://api.z.ai"
#define ZAI_PATH  "/api/paas/v4/chat/completions"
#define ZAI_MODEL "glm-4.6"
#define OA_BASE   "https://api.openai.com/v1"
#define OA_PATH   "/chat/completions"
#define OA_MODEL  "gpt-4o-mini"

static UmiLlmProvider parse_provider(const gchar *s){
  if(!s) return UMI_LLM_PROVIDER_ZAI;
  if(g_ascii_strcasecmp(s,"openai")==0) return UMI_LLM_PROVIDER_OPENAI;
  if(g_ascii_strcasecmp(s,"ollama")==0) return UMI_LLM_PROVIDER_OLLAMA;
  if(g_ascii_strcasecmp(s,"llama.cpp")==0) return UMI_LLM_PROVIDER_LLAMA_CPP;
  return UMI_LLM_PROVIDER_ZAI;
}

void umi_llm_cfg_init_from_env(UmiLlmCfg *cfg){
  g_return_if_fail(cfg);
  memset(cfg,0,sizeof(*cfg));
  const gchar *prov = g_getenv("LLM_PROVIDER");
  cfg->provider = parse_provider(prov);
  switch(cfg->provider){
    case UMI_LLM_PROVIDER_OPENAI:
      cfg->api_base = g_strdup(g_getenv("OPENAI_API_BASE") ? g_getenv("OPENAI_API_BASE") : OA_BASE);
      cfg->api_path = g_strdup(OA_PATH);
      cfg->api_key  = g_strdup(g_getenv("OPENAI_API_KEY"));
      cfg->model    = g_strdup(g_getenv("OPENAI_MODEL") ? g_getenv("OPENAI_MODEL") : OA_MODEL);
      break;
    case UMI_LLM_PROVIDER_ZAI:
    default:
      cfg->api_base = g_strdup(g_getenv("ZAI_API_BASE") ? g_getenv("ZAI_API_BASE") : ZAI_BASE);
      cfg->api_path = g_strdup(ZAI_PATH);
      cfg->api_key  = g_strdup(g_getenv("ZAI_API_KEY"));
      cfg->model    = g_strdup(g_getenv("ZAI_MODEL") ? g_getenv("ZAI_MODEL") : ZAI_MODEL);
      break;
  }
  const gchar *to = g_getenv("LLM_TIMEOUTMS");
  cfg->timeout_ms = (to && g_ascii_isdigit(to[0])) ? (guint)g_ascii_strtoull(to, NULL, 10) : 45000;
  cfg->stream = FALSE;
}

/* --- Request builders --- */
static guint env_topk(){
  const gchar *v = g_getenv("LLM_LOGPROBS_TOPK");
  if(!v || !*v) return 0; /* 0 disables */
  guint k = (guint)g_ascii_strtoull(v, NULL, 10);
  if(k>20) k=20; /* safety cap */
  return k;
}

static gchar* build_body_openai(const UmiLlmCfg *cfg, const gchar *sys, const gchar *usr, gboolean stream){
  JsonBuilder *b = json_builder_new();
  json_builder_begin_object(b);
  json_builder_set_member_name(b, "model");
  json_builder_add_string_value(b, cfg->model ? cfg->model : OA_MODEL);
  json_builder_set_member_name(b, "messages");
  json_builder_begin_array(b);
  if(sys && *sys){
    json_builder_begin_object(b);
    json_builder_set_member_name(b, "role");   json_builder_add_string_value(b, "system");
    json_builder_set_member_name(b, "content");json_builder_add_string_value(b, sys);
    json_builder_end_object(b);
  }
  json_builder_begin_object(b);
  json_builder_set_member_name(b, "role");   json_builder_add_string_value(b, "user");
  json_builder_set_member_name(b, "content");json_builder_add_string_value(b, usr?usr:"");
  json_builder_end_object(b);
  json_builder_end_array(b);
  json_builder_set_member_name(b, "stream");
  json_builder_add_boolean_value(b, stream?TRUE:FALSE);

  guint k = env_topk();
  if(k>0){
    json_builder_set_member_name(b, "logprobs");
    json_builder_begin_object(b);
    json_builder_set_member_name(b, "top_logprobs");
    json_builder_add_int_value(b, (gint)k);
    json_builder_end_object(b);
  }

  JsonGenerator *gen = json_generator_new();
  JsonNode *root = json_builder_get_root(b);
  json_generator_set_root(gen, root);
  gchar *payload = json_generator_to_data(gen, NULL);
  g_object_unref(gen); json_node_free(root); g_object_unref(b);
  return payload;
}

static gchar* build_body_zai(const UmiLlmCfg *cfg, const gchar *sys, const gchar *usr, gboolean stream){
  JsonBuilder *b = json_builder_new();
  json_builder_begin_object(b);
  json_builder_set_member_name(b, "model");
  json_builder_add_string_value(b, cfg->model ? cfg->model : ZAI_MODEL);
  json_builder_set_member_name(b, "messages");
  json_builder_begin_array(b);
  if(sys && *sys){
    json_builder_begin_object(b);
    json_builder_set_member_name(b, "role");   json_builder_add_string_value(b, "system");
    json_builder_set_member_name(b, "content");json_builder_add_string_value(b, sys);
    json_builder_end_object(b);
  }
  json_builder_begin_object(b);
  json_builder_set_member_name(b, "role");   json_builder_add_string_value(b, "user");
  json_builder_set_member_name(b, "content");json_builder_add_string_value(b, usr?usr:"");
  json_builder_end_object(b);
  json_builder_end_array(b);
  json_builder_set_member_name(b, "stream");
  json_builder_add_boolean_value(b, stream?TRUE:FALSE);
  JsonGenerator *gen = json_generator_new();
  JsonNode *root = json_builder_get_root(b);
  json_generator_set_root(gen, root);
  gchar *payload = json_generator_to_data(gen, NULL);
  g_object_unref(gen); json_node_free(root); g_object_unref(b);
  return payload;
}

/* --- Non-streaming parse --- */
static gchar* parse_openai_text(const gchar *resp_json){
  JsonParser *p = json_parser_new();
  if(!json_parser_load_from_data(p, resp_json, -1, NULL)){ g_object_unref(p); return NULL; }
  JsonNode *root = json_parser_get_root(p);
  JsonObject *obj = json_node_get_object(root);
  JsonArray *choices = obj? json_object_get_array_member(obj,"choices") : NULL;
  if(!choices || json_array_get_length(choices)==0){ g_object_unref(p); return NULL; }
  JsonObject *c0 = json_array_get_object_element(choices,0);
  JsonObject *msg = c0? json_object_get_object_member(c0,"message") : NULL;
  const gchar *content = msg? json_object_get_string_member(msg,"content") : NULL;
  gchar *out = content? g_strdup(content) : NULL;
  g_object_unref(p); return out;
}

static gchar* parse_zai_text(const gchar *resp_json){
  JsonParser *p = json_parser_new();
  if(!json_parser_load_from_data(p, resp_json, -1, NULL)){ g_object_unref(p); return NULL; }
  JsonNode *root = json_parser_get_root(p);
  JsonObject *obj = json_node_get_object(root);
  JsonArray *choices = obj? json_object_get_array_member(obj,"choices") : NULL;
  if(!choices || json_array_get_length(choices)==0){ g_object_unref(p); return NULL; }
  JsonObject *c0 = json_array_get_object_element(choices,0);
  JsonObject *msg = c0? json_object_get_object_member(c0,"message") : NULL;
  const gchar *content = msg? json_object_get_string_member(msg,"content") : NULL;
  gchar *out = content? g_strdup(content) : NULL;
  g_object_unref(p); return out;
}

bool umi_llm_chat_simple(const UmiLlmCfg *cfg,
                         const gchar *sys,
                         const gchar *usr,
                         gchar **out_text,
                         gchar *errbuf,
                         gsize errcap){
  g_return_val_if_fail(cfg && out_text, FALSE);
  *out_text = NULL;
  if(!cfg->api_key || !*cfg->api_key){ g_strlcpy(errbuf,"API key not set",errcap); return FALSE; }
  gchar *url = g_strdup_printf("%s%s", cfg->api_base, cfg->api_path);
  gchar *body = NULL; gchar *resp = NULL; gboolean ok = FALSE;
  switch(cfg->provider){
    case UMI_LLM_PROVIDER_OPENAI: body = build_body_openai(cfg, sys, usr, FALSE); break;
    case UMI_LLM_PROVIDER_ZAI:
    default:                       body = build_body_zai(cfg, sys, usr, FALSE); break;
  }
  ok = umi_http_post_json(url, cfg->api_key, body, cfg->timeout_ms, &resp, errbuf, errcap);
  if(!ok){ g_free(url); g_free(body); if(resp) g_free(resp); return FALSE; }
  switch(cfg->provider){
    case UMI_LLM_PROVIDER_OPENAI: *out_text = parse_openai_text(resp); break;
    case UMI_LLM_PROVIDER_ZAI:
    default:                      *out_text = parse_zai_text(resp); break;
  }
  g_free(resp); g_free(body); g_free(url);
  if(!*out_text){ g_strlcpy(errbuf,"No content in response",errcap); return FALSE; }
  return TRUE;
}

/* ---------------- Streaming (SSE) ---------------- */
typedef struct {
  UmiLlmOnTokenEx cb;
  gpointer ud;
  GString *buf;   /* accumulates across chunks */
  gboolean done;
} StreamCtx;

/* Public helper for tests: parse one OpenAI SSE payload line */
bool umi_llm_debug_parse_openai_sse_line(const gchar *json_line,
                                         gchar **out_fragment,
                                         UmiLlmTokenAlt **out_alts,
                                         guint *out_alts_n){
  if(out_fragment) *out_fragment=NULL;
  if(out_alts) *out_alts=NULL;
  if(out_alts_n) *out_alts_n=0;
  if(!json_line) return FALSE;

  JsonParser *p = json_parser_new();
  if(!json_parser_load_from_data(p, json_line, -1, NULL)){ g_object_unref(p); return FALSE; }
  JsonNode *root = json_parser_get_root(p);
  JsonObject *obj = json_node_get_object(root);
  if(!obj){ g_object_unref(p); return FALSE; }

  gchar *fragment = NULL;
  UmiLlmTokenAlt *alts = NULL; guint alts_n = 0;

  if(json_object_has_member(obj,"choices")){
    JsonArray *choices = json_object_get_array_member(obj,"choices");
    if(choices && json_array_get_length(choices)>0){
      JsonObject *c0 = json_array_get_object_element(choices,0);
      if(c0 && json_object_has_member(c0,"delta")){
        JsonObject *delta = json_object_get_object_member(c0,"delta");
        if(delta && json_object_has_member(delta,"content")){
          const gchar *frag = json_object_get_string_member(delta,"content");
          if(frag && *frag) fragment = g_strdup(frag);
        }
      }
      /* logprobs.content[0].top_logprobs[] if present */
      if(c0 && json_object_has_member(c0,"logprobs")){
        JsonObject *lp = json_object_get_object_member(c0,"logprobs");
        if(lp && json_object_has_member(lp,"content")){
          JsonArray *content = json_object_get_array_member(lp,"content");
          if(content && json_array_get_length(content)>0){
            JsonObject *entry0 = json_array_get_object_element(content,0);
            if(entry0){
              if(json_object_has_member(entry0,"top_logprobs")){
                JsonArray *tl = json_object_get_array_member(entry0,"top_logprobs");
                guint n = json_array_get_length(tl);
                if(n>0){
                  alts = g_new0(UmiLlmTokenAlt, n);
                  for(guint i=0;i<n;i++){
                    JsonObject *it = json_array_get_object_element(tl,i);
                    const gchar *tok = it? json_object_get_string_member(it,"token") : NULL;
                    double lg = it && json_object_has_member(it,"logprob") ? json_object_get_double_member(it,"logprob") : 0.0;
                    alts[i].token = tok? g_strdup(tok) : g_strdup("");
                    alts[i].logprob = lg;
                  }
                  alts_n = n;
                }
              } else if(json_object_has_member(entry0,"token")){
                /* fallback: content[].token + .logprob */
                const gchar *tok = json_object_get_string_member(entry0,"token");
                double lg = json_object_has_member(entry0,"logprob") ? json_object_get_double_member(entry0,"logprob") : 0.0;
                alts = g_new0(UmiLlmTokenAlt, 1);
                alts[0].token = g_strdup(tok?tok:"");
                alts[0].logprob = lg;
                alts_n = 1;
              }
            }
          }
        }
      }
    }
  }

  if(out_fragment) *out_fragment = fragment; else if(fragment) g_free(fragment);
  if(out_alts) { *out_alts = alts; if(out_alts_n) *out_alts_n = alts_n; }
  else { if(alts){ for(guint i=0;i<alts_n;i++) g_free(alts[i].token); g_free(alts);} }
  g_object_unref(p);
  return (fragment!=NULL) || (alts_n>0);
}

void umi_llm_free_alts(UmiLlmTokenAlt *alts, guint n){
  if(!alts) return;
  for(guint i=0;i<n;i++) g_free(alts[i].token);
  g_free(alts);
}

/* Handle a single SSE line "data: {{json}}". */
static void handle_sse_data_line(StreamCtx *sc, const gchar *json_line){
  gchar *fragment=NULL; UmiLlmTokenAlt *alts=NULL; guint alts_n=0;
  if(umi_llm_debug_parse_openai_sse_line(json_line, &fragment, &alts, &alts_n)){
    if(sc->cb) sc->cb(fragment, alts, alts_n, sc->ud);
  }
  if(fragment) g_free(fragment);
  umi_llm_free_alts(alts, alts_n);
}

static void on_chunk(const gchar *chunk, gsize n, gpointer ud){
  StreamCtx *sc = (StreamCtx*)ud;
  if(sc->done) return;
  /* normalize CRLF -> LF and append */
  for(gsize i=0;i<n;i++){
    char c = chunk[i];
    if(c==39) continue;
    g_string_append_c(sc->buf, c);
  }
  /* split on LF */
  gchar **lines = // ---- BEGIN FIXED SSE PARSER ----

  }
  g_strfreev(lines);
  g_string_set_size(sc->buf, 0);
}

bool umi_llm_chat_stream_ex(const UmiLlmCfg *cfg,
                            const gchar *sys,
                            const gchar *usr,
                            UmiLlmOnTokenEx on_token_ex,
                            gpointer ud,
                            gchar *errbuf, gsize errcap){
  g_return_val_if_fail(cfg && on_token_ex, FALSE);
  if(!cfg->api_key || !*cfg->api_key){ g_strlcpy(errbuf,"API key not set",errcap); return FALSE; }
  gchar *url = g_strdup_printf("%s%s", cfg->api_base, cfg->api_path);
  gchar *body = NULL; gboolean ok = FALSE;
  switch(cfg->provider){
    case UMI_LLM_PROVIDER_OPENAI: body = build_body_openai(cfg, sys, usr, TRUE); break;
    case UMI_LLM_PROVIDER_ZAI:
    default:                       body = build_body_zai(cfg, sys, usr, TRUE); break;
  }
  StreamCtx sc = {0};
  sc.cb = on_token_ex; sc.ud = ud; sc.buf = g_string_new(NULL); sc.done = FALSE;
  ok = umi_http_post_stream(url, cfg->api_key, body, cfg->timeout_ms, on_chunk, &sc, errbuf, errcap);
  g_string_free(sc.buf, TRUE);
  g_free(url); g_free(body);
  return ok;
}

bool umi_llm_chat_stream(const UmiLlmCfg *cfg,
                         const gchar *sys,
                         const gchar *usr,
                         UmiLlmOnToken on_token,
                         gpointer ud,
                         gchar *errbuf,
                         gsize errcap){
  if(!on_token) return FALSE;
  struct Adapt { UmiLlmOnToken f; gpointer ud; } ctx = { on_token, ud };
  void ex_cb(const gchar *frag, const UmiLlmTokenAlt *alts, guint alts_n, gpointer user){
    (void)alts; (void)alts_n;
    struct Adapt *c = (struct Adapt*)user;
    if(frag) c->f(frag, c->ud);
  }
  return umi_llm_chat_stream_ex(cfg, sys, usr, ex_cb, &ctx, errbuf, errcap);
}
