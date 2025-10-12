/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/llm_lab/llm_lab.c
 * PURPOSE: LLM Lab panel (pure C, GTK4). Loosely-coupled, builds even when
 *          LLM provider libraries are not linked (weak symbols + fallbacks).
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------
 * Key design points:
 * - Self-contained: includes only this module's public header + GLib/GTK.
 * - No BOM / non-ASCII surprises: keep this file strictly ASCII.
 * - No hard dependency on provider headers (llm.h). We declare minimal
 *   shims (types + weak functions). If backends are present at link time,
 *   the weak symbols will resolve; otherwise we gracefully degrade.
 * - Matches header contract:
 *     - umi_llm_lab_new_with_parent(GtkWindow *parent)
 *     - umi_llm_lab_present(GtkWidget *w)
 *   Note: your header defines `#define umi_llm_lab_new() umi_llm_lab_new_with_parent(NULL)`.
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <glib.h>
#include <math.h>        /* exp, log for entropy calculation */

#include "include/llm_lab.h"   /* local public header for this module */

/* ────────────────────────────────────────────────────────────────────────── */
/* Provider shims (weak) — keep us loosely coupled to any LLM backend.
 *
 * If your real provider headers are available, you can include them here
 * and remove these shims. Otherwise, we declare just enough to compile:
 *   - Token alternatives (string + logprob)
 *   - Minimal config (provider enum + streaming flag)
 *   - Weak symbols for cfg init and streaming call
 */

typedef struct {
    const char *token;   /* UTF-8 token text              */
    double      logprob; /* log(probability) for the token */
} UmiLlmTokenAlt;

typedef enum {
    UMI_LLM_PROVIDER_ZAI    = 1,
    UMI_LLM_PROVIDER_OPENAI = 2,
} UmiLlmProvider;

typedef struct {
    UmiLlmProvider provider;
    gboolean       stream;
    /* add fields as your backend requires, e.g., api_key, model, etc. */
} UmiLlmCfg;

/* NOTE: Weak symbols resolve to NULL if the backend isn't linked. */
__attribute__((weak)) void
umi_llm_cfg_init_from_env(UmiLlmCfg *cfg);

typedef gboolean (*UmiLlmOnTokenEx)(
    const gchar            *frag,
    const UmiLlmTokenAlt   *alts,
    guint                   alts_n,
    gpointer                user_data);

/* Weak streaming API: returns TRUE on success, FALSE on error (errbuf filled). */
__attribute__((weak)) gboolean
umi_llm_chat_stream_ex(const UmiLlmCfg *cfg,
                       const char      *system_prompt,
                       const char      *user_prompt,
                       void (*on_token_ex)(const gchar *, const UmiLlmTokenAlt *, guint, gpointer),
                       gpointer         user_data,
                       char            *errbuf,
                       size_t           errbuf_sz);

/* ────────────────────────────────────────────────────────────────────────── */
/* Internal model (widget state) */

typedef struct {
    /* Controls */
    GtkComboBoxText *provider;
    GtkSwitch       *stream_sw;
    GtkCheckButton  *show_alts;
    GtkEntry        *entry;

    /* Output areas */
    GtkTextView   *out_view;
    GtkTextBuffer *buf;

    GtkTextView   *alts_view;
    GtkTextBuffer *alts_buf;
} LlmLab;

/* Append text to a GtkTextBuffer safely */
static void
append_text(GtkTextBuffer *b, const gchar *txt)
{
    if (!b || !txt) return;
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(b, &end);
    gtk_text_buffer_insert(b, &end, txt, -1);
}

/* Compute and show entropy of a (capped) top-k set from logprobs. */
static void
show_entropy(GtkTextBuffer *b, const UmiLlmTokenAlt *alts, guint n)
{
    if (!b) return;
    if (!alts || n == 0) {
        append_text(b, "(no alternatives)\n");
        return;
    }

    /* Use log-sum-exp for stability. */
    double maxlg = alts[0].logprob;
    for (guint i = 1; i < n; i++)
        if (alts[i].logprob > maxlg) maxlg = alts[i].logprob;

    double Z = 0.0;
    double probs[64];
    const guint m = (n > 64) ? 64u : n;
    for (guint i = 0; i < m; i++) {
        probs[i] = exp(alts[i].logprob - maxlg);
        Z += probs[i];
    }
    if (Z <= 0.0) Z = 1.0;
    for (guint i = 0; i < m; i++)
        probs[i] /= Z;

    double H = 0.0;
    for (guint i = 0; i < m; i++)
        if (probs[i] > 0.0) H -= probs[i] * log(probs[i]);

    char line[128];
    g_snprintf(line, sizeof line, "entropy(H): %.3f\n", H);
    append_text(b, line);
}

/* Streaming callback: prints fragments and (optionally) top-k alt tokens. */
static void
on_stream_token_ex(const gchar *frag,
                   const UmiLlmTokenAlt *alts,
                   guint alts_n,
                   gpointer ud)
{
    LlmLab *lab = (LlmLab*)ud;
    if (!lab) return;

    if (frag && *frag)
        append_text(lab->buf, frag);

    if (gtk_check_button_get_active(lab->show_alts) && alts && alts_n > 0) {
        append_text(lab->alts_buf, "— top-k —\n");
        for (guint i = 0; i < alts_n; i++) {
            gchar line[256];
            g_snprintf(line, sizeof line, "  %s  (%.3f)\n", alts[i].token, alts[i].logprob);
            append_text(lab->alts_buf, line);
        }
        show_entropy(lab->alts_buf, alts, alts_n);
    }

    /* Keep UI responsive during streaming. */
    while (g_main_context_pending(NULL))
        g_main_context_iteration(NULL, FALSE);
}

/* Send handler: builds cfg, selects provider, and (if available) streams chat. */
static void
on_send(GtkButton *btn, gpointer user_data)
{
    (void)btn;
    LlmLab *lab = (LlmLab*)user_data;
    if (!lab) return;

    const gchar *q = gtk_editable_get_text(GTK_EDITABLE(lab->entry));
    if (!q || !*q) return;

    /* Clear previous output. */
    gtk_text_buffer_set_text(lab->buf, "", -1);
    gtk_text_buffer_set_text(lab->alts_buf, "", -1);

    /* If backend isn’t linked, show a friendly message and bail. */
    if (!umi_llm_chat_stream_ex) {
        append_text(lab->buf, "⚠ LLM backend not linked. Please enable provider library.\n");
        return;
    }

    /* Build configuration (weak init; if missing, set minimal defaults). */
    UmiLlmCfg cfg;
    if (umi_llm_cfg_init_from_env) {
        umi_llm_cfg_init_from_env(&cfg);
    } else {
        cfg.provider = UMI_LLM_PROVIDER_ZAI;
        cfg.stream   = TRUE;
    }

    /* Provider selection (GtkComboBoxText API returns newly-allocated string). */
    char *prov = gtk_combo_box_text_get_active_text(lab->provider);
    if (prov) {
        if (g_ascii_strcasecmp(prov, "openai") == 0) cfg.provider = UMI_LLM_PROVIDER_OPENAI;
        else                                         cfg.provider = UMI_LLM_PROVIDER_ZAI;
        g_free(prov);
    }

    cfg.stream = gtk_switch_get_active(lab->stream_sw);

    /* Stream! */
    char err[256] = {0};
    append_text(lab->buf, "▶ Streaming…\n");

    const char *system_prompt = "You are a helpful IDE assistant.";
    gboolean ok = umi_llm_chat_stream_ex(&cfg, system_prompt, q,
                                         on_stream_token_ex, lab,
                                         err, sizeof err);

    if (!ok) {
        append_text(lab->buf, "⚠ ");
        append_text(lab->buf, err[0] ? err : "unknown error");
        append_text(lab->buf, "\n");
    } else {
        append_text(lab->buf, "\n✅ done\n");
    }
}

/* Build the widget (grid layout) */
static GtkWidget *
build_lab(LlmLab **out_state)
{
    LlmLab *lab = g_new0(LlmLab, 1);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing   (GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 6);
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_widget_set_vexpand(grid, TRUE);

    lab->provider = GTK_COMBO_BOX_TEXT(gtk_combo_box_text_new());
    gtk_combo_box_text_append_text(lab->provider, "zai");
    gtk_combo_box_text_append_text(lab->provider, "openai");
    gtk_combo_box_set_active(GTK_COMBO_BOX(lab->provider), 0);

    lab->stream_sw = GTK_SWITCH(gtk_switch_new());
    gtk_switch_set_active(lab->stream_sw, TRUE);

    lab->show_alts = GTK_CHECK_BUTTON(
        gtk_check_button_new_with_label("Show token alternatives (when available)")
    );
    gtk_check_button_set_active(lab->show_alts, TRUE);

    GtkWidget *send = gtk_button_new_with_label("Send");
    g_signal_connect(send, "clicked", G_CALLBACK(on_send), lab);

    lab->entry = GTK_ENTRY(gtk_entry_new());
    gtk_editable_set_text(GTK_EDITABLE(lab->entry), "Explain what this project does.");

    /* Output area */
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scroll, TRUE);
    lab->out_view = GTK_TEXT_VIEW(gtk_text_view_new());
    lab->buf = gtk_text_view_get_buffer(lab->out_view);
    gtk_text_view_set_wrap_mode(lab->out_view, GTK_WRAP_WORD_CHAR);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), GTK_WIDGET(lab->out_view));

    /* Alternatives area */
    GtkWidget *alts_scroll = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(alts_scroll, TRUE);
    lab->alts_view = GTK_TEXT_VIEW(gtk_text_view_new());
    lab->alts_buf = gtk_text_view_get_buffer(lab->alts_view);
    gtk_text_view_set_wrap_mode(lab->alts_view, GTK_WRAP_WORD_CHAR);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(alts_scroll), GTK_WIDGET(lab->alts_view));

    /* Layout:
     * row 0: [provider] [stream switch] [show_alts]
     * row 1: [entry.................]   [send]
     * row 2: [out scroll .........]    [alts scroll]
     */
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(lab->provider), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(lab->stream_sw), 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(lab->show_alts), 2, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(lab->entry),     0, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), send,                        2, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), scroll,                      0, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), alts_scroll,                 2, 2, 1, 1);

    if (out_state) *out_state = lab;
    return grid;
}

/* ────────────────────────────────────────────────────────────────────────── */
/* Public API (matches include/llm_lab.h) */

GtkWidget *
umi_llm_lab_new_with_parent(GtkWindow *parent)
{
    (void)parent; /* reserved for future parent-aware behavior */
    LlmLab *st = NULL;
    GtkWidget *w = build_lab(&st);
    /* Optionally store st on the widget if you need to look it up later:
       g_object_set_data_full(G_OBJECT(w), "llm.lab.state", st, g_free); */
    return w;
}

void
umi_llm_lab_present(GtkWidget *w)
{
    if (!w) return;
    GtkRoot *root = gtk_widget_get_root(w);
    if (GTK_IS_WINDOW(root)) {
        gtk_window_present(GTK_WINDOW(root));
    } else {
        /* If not inside a window yet, try to grab keyboard focus. */
        gtk_widget_grab_focus(w);
    }
}
/*--- end of file ---*/