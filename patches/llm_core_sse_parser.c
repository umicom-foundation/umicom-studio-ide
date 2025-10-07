// ---- BEGIN FIXED SSE PARSER ----
static void umi__parse_sse_buffer(UmiStreamCtx *sc) {
    if (!sc || !sc->buf) return;

    gchar **lines = g_strsplit(sc->buf->str, "\n", 0);
    if (!lines) return;

    for (guint i = 0; lines[i]; i++) {
        gchar *s = lines[i];
        size_t len = strlen(s);
        if (len && s[len-1] == '\r') s[len-1] = 0;

        if (g_str_has_prefix(s, "data: ")) {
            const gchar *payload = s + 6;
            if (g_str_equal(payload, "[DONE]")) {
                sc->done = true;
                continue;
            }
            if (sc->on_chunk) sc->on_chunk(payload, sc->user);
        }
    }
    g_strfreev(lines);
    g_string_truncate(sc->buf, 0);
}
// ---- END FIXED SSE PARSER ----
