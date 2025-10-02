#ifndef UMICOM_LLM_HTTP_H
#define UMICOM_LLM_HTTP_H
/* Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT */

#include <stdbool.h>
#include <glib.h>

/* Simple POST JSON; allocates *out_body (caller g_free()). */
bool umi_http_post_json(const gchar *url,
                        const gchar *bearer_token_or_null,
                        const gchar *json_body,
                        guint        timeout_ms,
                        gchar      **out_body,
                        gchar       *errbuf,
                        gsize        errcap);

/* Streaming POST (SSE-like): forwards raw chunks. */
typedef void (*UmiHttpOnChunk)(const gchar *chunk, gsize n, gpointer ud);

bool umi_http_post_stream(const gchar *url,
                          const gchar *bearer_token_or_null,
                          const gchar *json_body,
                          guint        timeout_ms,
                          UmiHttpOnChunk on_chunk,
                          gpointer     ud,
                          gchar       *errbuf,
                          gsize        errcap);

#endif
