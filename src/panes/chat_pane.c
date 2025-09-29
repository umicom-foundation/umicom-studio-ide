
/*
 * Umicom Studio IDE - Chat Pane (GTK4)
 * Author: Umicom Foundation (MIT)
 */
#include "chat_pane.h"
#include <glib.h>
#include <glib/gi18n.h>
#include <libsoup/soup.h>
#include <json-glib/json-glib.h>

typedef struct {
    GtkWidget   *root;
    GtkTextView *log;
    GtkEntry    *entry;
    GtkButton   *send;
    char        *endpoint;
    SoupSession *session;
} ChatPane;

static void chat_append_text(ChatPane* cp, const char* prefix, const char* text) {
    GtkTextBuffer *buf = gtk_text_view_get_buffer(cp->log);
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buf, &end);
    if (prefix && *prefix) {
        gtk_text_buffer_insert(buf, &end, prefix, -1);
        gtk_text_buffer_insert(buf, &end, ": ", -1);
    }
    gtk_text_buffer_insert(buf, &end, text, -1);
    gtk_text_buffer_insert(buf, &end, "\n", -1);
}

static void on_send_clicked(GtkButton *btn, gpointer user_data) {
    ChatPane* cp = (ChatPane*)user_data;
    const char* prompt = gtk_editable_get_text(GTK_EDITABLE(cp->entry));
    if (!prompt || !*prompt) return;

    chat_append_text(cp, "You", prompt);
    gtk_editable_set_text(GTK_EDITABLE(cp->entry), "");

    /* Build JSON body */
    JsonBuilder *b = json_builder_new();
    json_builder_begin_object(b);
    json_builder_set_member_name(b, "prompt");
    json_builder_add_string_value(b, prompt);
    json_builder_end_object(b);
    JsonGenerator *gen = json_generator_new();
    JsonNode *root = json_builder_get_root(b);
    json_generator_set_root(gen, root);
    gsize len = 0;
    gchar *body = json_generator_to_data(gen, &len);
    g_object_unref(gen);
    json_node_free(root);
    g_object_unref(b);

    /* HTTP POST */
    const char* url = cp->endpoint ? cp->endpoint : "http://127.0.0.1:8750/chat";
    SoupMessage *msg = soup_message_new("POST", url);
    soup_message_set_request_body_from_bytes(msg, "application/json", g_bytes_new_take(body, len)); /* body owned */
    soup_session_send_and_read_async(cp->session, msg, G_PRIORITY_DEFAULT, NULL,
        (GAsyncReadyCallback) (+[] (SoupSession* session, GAsyncResult* res, gpointer u) {
            ChatPane* cp = (ChatPane*)u;
            GError* err = NULL;
            GBytes* bytes = soup_session_send_and_read_finish(session, res, &err);
            if (err) {
                chat_append_text(cp, "ERR", err->message);
                g_error_free(err);
                return;
            }
            gsize sz = 0;
            const char* data = (const char*)g_bytes_get_data(bytes, &sz);
            /* Parse JSON { ok: bool, reply: string } */
            JsonParser *parser = json_parser_new();
            if (json_parser_load_from_data(parser, data, (gssize)sz, NULL)) {
                JsonNode *root = json_parser_get_root(parser);
                if (JSON_NODE_HOLDS_OBJECT(root)) {
                    JsonObject *obj = json_node_get_object(root);
                    const char* reply = json_object_get_string_member_with_default(obj, "reply", NULL);
                    if (reply) {
                        chat_append_text(cp, "AI", reply);
                    } else {
                        chat_append_text(cp, "AI", "[no reply field]");
                    }
                } else {
                    chat_append_text(cp, "AI", "[invalid JSON root]");
                }
            } else {
                chat_append_text(cp, "AI", "[invalid JSON]");
            }
            g_object_unref(parser);
            g_bytes_unref(bytes);
        }),
        cp
    );
    g_object_unref(msg);
}

GtkWidget* chat_pane_new(void) {
    ChatPane* cp = g_new0(ChatPane, 1);
    cp->session = soup_session_new();
    cp->endpoint = g_strdup("http://127.0.0.1:8750/chat");

    /* Load UI from resource (ensure data/ui/chat.ui is in gresource) */
    GtkBuilder* b = gtk_builder_new_from_resource("/com/umicom/studio/ui/chat.ui");
    cp->root  = GTK_WIDGET(gtk_builder_get_object(b, "chat_root"));
    cp->log   = GTK_TEXT_VIEW(gtk_builder_get_object(b, "chat_log"));
    cp->entry = GTK_ENTRY(gtk_builder_get_object(b, "chat_entry"));
    cp->send  = GTK_BUTTON(gtk_builder_get_object(b, "chat_send"));
    g_object_ref(cp->root); /* keep widget after builder is unreffed */
    g_object_unref(b);

    g_signal_connect(cp->send, "clicked", G_CALLBACK(on_send_clicked), cp);
    return cp->root;
}

void chat_pane_set_endpoint(GtkWidget* pane, const char* endpoint_url) {
    /* Store on the instance via object data */
    ChatPane* cp = NULL;
    /* For simplicity, stash ChatPane struct as object data once on first call */
    cp = g_object_get_data(G_OBJECT(pane), "ustudio-chatpane");
    if (!cp) {
        /* Rebuild pointers by walking children */
        cp = g_new0(ChatPane, 1);
        cp->root = pane;
        /* Crude rebuild: find by template ids */
        GtkWidget *entry = gtk_widget_lookup(pane, "chat_entry");
        GtkWidget *tv    = gtk_widget_lookup(pane, "chat_log");
        GtkWidget *btn   = gtk_widget_lookup(pane, "chat_send");
        cp->entry = GTK_ENTRY(entry);
        cp->log   = GTK_TEXT_VIEW(tv);
        cp->send  = GTK_BUTTON(btn);
        cp->session = soup_session_new();
        g_signal_connect(cp->send, "clicked", G_CALLBACK(on_send_clicked), cp);
        g_object_set_data_full(G_OBJECT(pane), "ustudio-chatpane", cp, (GDestroyNotify)g_free);
    }
    g_free(cp->endpoint);
    cp->endpoint = g_strdup(endpoint_url);
}
