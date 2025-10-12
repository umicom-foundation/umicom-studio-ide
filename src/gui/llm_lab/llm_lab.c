/* -----------------------------------------------------------------------------
 * Umicom Studio IDE
 * PURPOSE: Core sources for Umicom Studio IDE.
 * Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
 * Last updated: 2025-10-11
 * ---------------------------------------------------------------------------*/
ï»¿/* LLM Lab (GTK) â€” Token Inspector (entropy + top-k) */
#include <llm_lab.h>
#include <llm.h>
#include <glib/gi18n.h>
#include <math.h>

typedef struct {
  GtkTextView  *out_view;
  GtkTextBuffer*buf;
  GtkTextView  *alts_view;
  GtkTextBuffer*alts_buf;
  GtkEntry     *entry;
  GtkComboBoxText *provider;
  GtkSwitch    *stream_sw;
  GtkCheckButton *show_alts;
} LlmLab;

static void append_text(GtkTextBuffer *b, const gchar *txt){
  GtkTextIter end; gtk_text_buffer_get_end_iter(b,&end);
  gtk_text_buffer_insert(b,&end, txt, -1);
}

static void show_entropy(GtkTextBuffer *b, const UmiLlmTokenAlt *alts, guint n){
  if(!alts || n==0){ append_text(b, "(no alternatives)\n"); return; }
  /* compute normalized probs from logprobs */
  double maxlg = alts[0].logprob;
  for(guint i=1;i<n;i++) if(alts[i].logprob>maxlg) maxlg = alts[i].logprob;
  double Z=0.0; double probs[64];
  guint m = n>64?64:n;
  for(guint i=0;i<m;i++){ probs[i] = exp(alts[i].logprob - maxlg); Z += probs[i]; }
  for(guint i=0;i<m;i++) probs[i] /= (Z>0?Z:1.0);
  double H=0.0; for(guint i=0;i<m;i++){ if(probs[i]>0) H -= probs[i]*log(probs[i]); }
  char line[128]; g_snprintf(line,sizeof line,"entropy(H): %.3f\n", H);
  append_text(b, line);
}

static void on_stream_token_ex(const gchar *frag, const UmiLlmTokenAlt *alts, guint alts_n, gpointer ud){
  LlmLab *lab = (LlmLab*)ud;
  if(frag && *frag) append_text(lab->buf, frag);
  if(gtk_check_button_get_active(lab->show_alts) && alts && alts_n>0){
    append_text(lab->alts_buf, "â€” top-k â€”\n");
    for(guint i=0;i<alts_n;i++){
      gchar line[256]; g_snprintf(line,sizeof line,"  %s  (%.3f)\n", alts[i].token, alts[i].logprob);
      append_text(lab->alts_buf, line);
    }
    show_entropy(lab->alts_buf, alts, alts_n);
  }
  while (g_main_context_pending(NULL)) g_main_context_iteration(NULL, FALSE);
}

static void on_send(GtkButton *btn, gpointer user_data){
  LlmLab *lab = (LlmLab*)user_data;
  const gchar *q = gtk_editable_get_text(GTK_EDITABLE(lab->entry));
  if(!q || !*q) return;

  UmiLlmCfg cfg; umi_llm_cfg_init_from_env(&cfg);
  const gchar *prov = gtk_combo_box_text_get_active_text(lab->provider);
  if(prov && g_ascii_strcasecmp(prov,"openai")==0) cfg.provider = UMI_LLM_PROVIDER_OPENAI;
  else cfg.provider = UMI_LLM_PROVIDER_ZAI;
  cfg.stream = TRUE;

  gtk_text_buffer_set_text(lab->buf, "", -1);
  gtk_text_buffer_set_text(lab->alts_buf, "", -1);

  gchar err[256]={0};
  append_text(lab->buf, "â–¶ Streamingâ€¦\n");
  if(!umi_llm_chat_stream_ex(&cfg, "You are a helpful IDE assistant.", q, on_stream_token_ex, lab, err, sizeof err)){
    append_text(lab->buf, "âš  "); append_text(lab->buf, err); append_text(lab->buf, "\n");
  } else {
    append_text(lab->buf, "\nâœ… done\n");
  }
}

GtkWidget* umi_llm_lab_new(void){
  LlmLab *lab = g_new0(LlmLab,1);
  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 6);
  gtk_widget_set_hexpand(grid, TRUE);
  gtk_widget_set_vexpand(grid, TRUE);

  lab->provider = GTK_COMBO_BOX_TEXT(gtk_combo_box_text_new());
  gtk_combo_box_text_append_text(lab->provider, "zai");
  gtk_combo_box_text_append_text(lab->provider, "openai");
  gtk_combo_box_set_active(GTK_COMBO_BOX(lab->provider), 0);

  lab->stream_sw = GTK_SWITCH(gtk_switch_new());
  gtk_switch_set_active(lab->stream_sw, TRUE);

  lab->show_alts = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Show token alternatives (when available)"));
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

  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(lab->provider), 0,0,1,1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(lab->stream_sw), 1,0,1,1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(lab->show_alts), 2,0,1,1);
  gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(lab->entry),    0,1,2,1);
  gtk_grid_attach(GTK_GRID(grid), send,                       2,1,1,1);
  gtk_grid_attach(GTK_GRID(grid), scroll,                     0,2,2,1);
  gtk_grid_attach(GTK_GRID(grid), alts_scroll,                2,2,1,1);
  return grid;
}

void umi_llm_lab_focus(GtkWidget *lab){
  if(!lab) return;
  gtk_widget_grab_focus(lab);
}
