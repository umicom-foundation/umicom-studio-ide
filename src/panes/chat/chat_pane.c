/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/panes/chat_pane.c
 * PURPOSE: Simple chat pane UI (GTK4) + async HTTP using libsoup-3.0
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *
 * IMPORTANT
 * =========
 * This file addresses the latest compile errors:
 *   - 'gtk_buildable_get_name' / 'gtk_buildable_set_name' are not available
 *     in GTK4 (they were removed). We now use the supported GTK4 API:
 *       * gtk_widget_get_name()
 *       * gtk_widget_set_name()
 *     to label widgets and to traverse by name.
 *
 *   - We keep a defensive fallback that stores direct pointers on the root
 *     container via g_object_set_data() so code can also retrieve widgets
 *     by known keys without relying on names.
 *
 *   - We continue to avoid any C++-style lambdas for libsoup async. A proper
 *     C callback (on_soup_done) is used.
 *
 * Notes
 * -----
 * * The code is intentionally verbose and documented for future maintenance.
 * * Replace the example GET call to postman-echo with your real backend.
 * * All original project credits/comments must remain; do not remove them.
 *---------------------------------------------------------------------------*/

#include <glib.h>
#include <gtk/gtk.h>        /* GtkWidget, GtkTextView, GtkTextBuffer, etc. */
#include <libsoup/soup.h>   /* libsoup-3.0 async HTTP client */

/*-----------------------------------------------------------------------------
 * Local model: ChatPane
 * Holds pointers to child widgets we manipulate frequently.
 *---------------------------------------------------------------------------*/
typedef struct ChatPane {
    GtkTextView *log;     /* Multiline log area where we append messages       */
    GtkEntry    *entry;   /* Single-line entry where the user types a prompt   */
    GtkButton   *send;    /* Button to submit the prompt                       */
} ChatPane;

/*-----------------------------------------------------------------------------
 * Helper: depth-first name lookup (GTK4)
 *
 * GTK4 removed gtk_buildable_get_name()/set_name(). The supported, documented
 * way to tag and query a widget's name is:
 *   - gtk_widget_set_name()
 *   - gtk_widget_get_name()
 *
 * This helper does two things:
 *   1) If the *root* widget has GObject data with the given key (set with
 *      g_object_set_data()), return that pointer immediately (O(1)).
 *   2) Otherwise, recursively traverse the widget subtree and return the first
 *      widget whose gtk_widget_get_name() matches the requested name.
 *---------------------------------------------------------------------------*/
static GtkWidget *gtk_widget_lookup(GtkWidget *root, const char *name) {
    if (!root || !name) return NULL;

    /* Fast-path: explicit object data on the root (acts like an index). */
    gpointer direct = g_object_get_data(G_OBJECT(root), name);
    if (direct) return GTK_WIDGET(direct);

    /* Traverse by GTK4 widget names. */
    const char *wname = gtk_widget_get_name(root);
    if (wname && g_strcmp0(wname, name) == 0)
        return root;

    for (GtkWidget *child = gtk_widget_get_first_child(root);
         child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = gtk_widget_lookup(child, name);
        if (found) return found;
    }
    return NULL;
}

/*-----------------------------------------------------------------------------
 * Helper: append a single line into the log text view.
 * If `prefix` is provided, we render "prefix: text", otherwise just `text`.
 *---------------------------------------------------------------------------*/
static void chat_append(ChatPane *cp, const char *prefix, const char *text) {
    if (!cp || !cp->log) return;

    GtkTextBuffer *buf = gtk_text_view_get_buffer(cp->log);
    if (!buf) return;

    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buf, &end);

    if (prefix) {
        gtk_text_buffer_insert(buf, &end, prefix, -1);
        gtk_text_buffer_insert(buf, &end, ": ", -1);
    }
    gtk_text_buffer_insert(buf, &end, text ? text : "", -1);
    gtk_text_buffer_insert(buf, &end, "\n", -1);
}

/*-----------------------------------------------------------------------------
 * libsoup async callback (C, not C++)
 * Finishes the async request, logs the response or error.
 *---------------------------------------------------------------------------*/
static void on_soup_done(GObject *source, GAsyncResult *res, gpointer user_data) {
    ChatPane *cp = (ChatPane*)user_data;
    g_autoptr(GError) err = NULL;

    /* Finish the async read into a GBytes */
    GBytes *body = soup_session_send_and_read_finish(SOUP_SESSION(source), res, &err);
    if (err) {
        chat_append(cp, "Error", err->message);
        return;
    }

    gsize len = 0;
    const char *data = (const char*)g_bytes_get_data(body, &len);
    chat_append(cp, "Assistant", (data && len) ? data : "(empty response)");
    g_bytes_unref(body);
}

/*-----------------------------------------------------------------------------
 * Signal handler: user clicked Send
 * - Reads entry text
 * - Appends local echo to log
 * - Issues a minimal GET to a placeholder endpoint (replace with your real one)
 *---------------------------------------------------------------------------*/
static void on_send_clicked(GtkButton *btn, gpointer user_data) {
    (void)btn; /* Unused in this callback, but keep signature tidy */
    ChatPane *cp = (ChatPane*)user_data;
    if (!cp || !cp->entry) return;

    const char *prompt = gtk_editable_get_text(GTK_EDITABLE(cp->entry));
    if (!prompt || !*prompt) return;

    /* Echo user's text in the log first */
    chat_append(cp, "You", prompt);

    /* Build a simple request: replace the URL with your backend endpoint.
     * For a POST/JSON payload to an LLM API, use SoupMessage + body variants.
     */
    g_autoptr(SoupSession) session = soup_session_new();
    char *escaped = g_uri_escape_string(prompt, NULL, FALSE);
    char *url = g_strdup_printf("https://postman-echo.com/get?text=%s", escaped ? escaped : "");
    g_free(escaped);

    g_autoptr(SoupMessage) msg = soup_message_new("GET", url);
    g_free(url);

    /* Async send: callback will log the response. */
    soup_session_send_and_read_async(session, msg, G_PRIORITY_DEFAULT, NULL, on_soup_done, cp);
}

/*-----------------------------------------------------------------------------
 * Public constructor: builds the chat pane UI and wires signals.
 * - Returns a GtkWidget* container that holds:
 *     [ScrolledWindow(TextView)] + [HBox: Entry, Send Button]
 * - Assigns widget names (GTK4 API) so gtk_widget_lookup() works.
 * - Also stores direct pointers on the root with g_object_set_data() using the
 *   same keys ("chat_entry", "chat_log", "chat_send") for fast retrieval.
 *---------------------------------------------------------------------------*/
GtkWidget* umi_chat_pane_new(void) {
    /* Root container: vertical box */
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_name(root, "chat_pane_root"); /* GTK4 name tag */
    gtk_widget_set_hexpand(root, TRUE);
    gtk_widget_set_vexpand(root, TRUE);

    /* Text log inside a scrolled window */
    GtkWidget *sw = gtk_scrolled_window_new();
    gtk_widget_set_hexpand(sw, TRUE);
    gtk_widget_set_vexpand(sw, TRUE);
    gtk_box_append(GTK_BOX(root), sw);

    GtkWidget *tv = gtk_text_view_new();
    gtk_widget_set_name(tv, "chat_log"); /* GTK4 name tag */
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tv), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(tv), FALSE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(sw), tv);

    /* Entry + Send button row */
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(root), row);

    GtkWidget *entry = gtk_entry_new();
    gtk_widget_set_name(entry, "chat_entry"); /* GTK4 name tag */
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_box_append(GTK_BOX(row), entry);

    GtkWidget *send = gtk_button_new_with_label("Send");
    gtk_widget_set_name(send, "chat_send"); /* GTK4 name tag */
    gtk_box_append(GTK_BOX(row), send);

    /* Allocate and wire the ChatPane controller */
    ChatPane *cp = g_new0(ChatPane, 1);
    cp->log   = GTK_TEXT_VIEW(tv);
    cp->entry = GTK_ENTRY(entry);
    cp->send  = GTK_BUTTON(send);

    g_signal_connect(send, "clicked", G_CALLBACK(on_send_clicked), cp);

    /* Store controller and direct child pointers on the root for fast lookup. */
    g_object_set_data_full(G_OBJECT(root), "umi-chat-pane", cp, g_free);
    g_object_set_data(G_OBJECT(root), "chat_entry", entry);
    g_object_set_data(G_OBJECT(root), "chat_log", tv);
    g_object_set_data(G_OBJECT(root), "chat_send", send);

    return root;
}

/*-----------------------------------------------------------------------------
 * Optional helper (if other code uses a generic container and then looks up
 * the child widgets by name). This mirrors how your code referenced names.
 *---------------------------------------------------------------------------*/
void umi_chat_pane_wire(GtkWidget *pane) {
    if (!pane) return;

    ChatPane *cp = g_object_get_data(G_OBJECT(pane), "umi-chat-pane");
    if (!cp) {
        /* If we didn't get one, reconstruct via object data or names. */
        GtkWidget *entry = gtk_widget_lookup(pane, "chat_entry");
        GtkWidget *tv    = gtk_widget_lookup(pane, "chat_log");
        GtkWidget *btn   = gtk_widget_lookup(pane, "chat_send");

        if (entry && tv && btn) {
            cp = g_new0(ChatPane, 1);
            cp->log   = GTK_TEXT_VIEW(tv);
            cp->entry = GTK_ENTRY(entry);
            cp->send  = GTK_BUTTON(btn);
            g_object_set_data_full(G_OBJECT(pane), "umi-chat-pane", cp, g_free);
            g_signal_connect(cp->send, "clicked", G_CALLBACK(on_send_clicked), cp);
        }
    }
}
