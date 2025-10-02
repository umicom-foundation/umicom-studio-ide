/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/prefs.c
 * PURPOSE: Preferences dialog implementation (pure C callbacks)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "prefs.h"
#include "status_util.h"
#include <json-glib/json-glib.h>

typedef struct {
  UmiSettings *s;
  GtkWidget *dd_theme;
  GtkWidget *spin_font;
  GtkWidget *e_umicc;
  GtkWidget *e_ua;
  GtkWidget *e_rg;
  GtkWidget *chk_auto;
  GtkWidget *spin_auto;
} PrefsCtx;

static GtkWidget *mk_labeled(GtkWidget **out_entry, const char *lbl, const char *text){
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_append(GTK_BOX(box), gtk_label_new(lbl));
  GtkWidget *e = gtk_entry_new();
  gtk_editable_set_text(GTK_EDITABLE(e), text?text:"");
  gtk_box_append(GTK_BOX(box), e);
  if(out_entry) *out_entry = e;
  return box;
}

static UmiSettings *defaults(void){
  UmiSettings *s = g_new0(UmiSettings,1);
  s->theme = g_strdup("light");
  s->font_size = 12;
  s->umicc_path = g_strdup("");
  s->uaengine_path = g_strdup("");
  s->ripgrep_path = g_strdup("");
  s->autosave_enabled = TRUE;
  s->autosave_interval_sec = 30;
  return s;
}

static const char *SETTINGS_JSON = "config/settings.json";

UmiSettings* umi_settings_load(void){
  gchar *txt=NULL; gsize len=0;
  if(!g_file_get_contents(SETTINGS_JSON,&txt,&len,NULL)) return defaults();
  JsonParser *p=json_parser_new();
  if(!json_parser_load_from_data(p,txt,(gssize)len,NULL)){ g_object_unref(p); g_free(txt); return defaults(); }
  JsonObject *o=json_node_get_object(json_parser_get_root(p));
  UmiSettings *s = defaults();
  if(json_object_has_member(o,"theme")){ g_free(s->theme); s->theme = g_strdup(json_object_get_string_member(o,"theme")); }
  if(json_object_has_member(o,"font_size")) s->font_size = json_object_get_int_member(o,"font_size");
  if(json_object_has_member(o,"umicc_path")){ g_free(s->umicc_path); s->umicc_path = g_strdup(json_object_get_string_member(o,"umicc_path")); }
  if(json_object_has_member(o,"uaengine_path")){ g_free(s->uaengine_path); s->uaengine_path = g_strdup(json_object_get_string_member(o,"uaengine_path")); }
  if(json_object_has_member(o,"ripgrep_path")){ g_free(s->ripgrep_path); s->ripgrep_path = g_strdup(json_object_get_string_member(o,"ripgrep_path")); }
  if(json_object_has_member(o,"autosave_enabled")) s->autosave_enabled = json_object_get_boolean_member(o,"autosave_enabled");
  if(json_object_has_member(o,"autosave_interval_sec")) s->autosave_interval_sec = json_object_get_int_member(o,"autosave_interval_sec");
  g_object_unref(p); g_free(txt);
  return s;
}

gboolean umi_settings_save(const UmiSettings *s){
  if(!s) return FALSE;
  g_mkdir_with_parents("config",0755);
  JsonBuilder *b=json_builder_new(); json_builder_begin_object(b);
  json_builder_set_member_name(b,"theme"); json_builder_add_string_value(b, s->theme?s->theme:"light");
  json_builder_set_member_name(b,"font_size"); json_builder_add_int_value(b, s->font_size);
  json_builder_set_member_name(b,"umicc_path"); json_builder_add_string_value(b, s->umicc_path?s->umicc_path:"");
  json_builder_set_member_name(b,"uaengine_path"); json_builder_add_string_value(b, s->uaengine_path?s->uaengine_path:"");
  json_builder_set_member_name(b,"ripgrep_path"); json_builder_add_string_value(b, s->ripgrep_path?s->ripgrep_path:"");
  json_builder_set_member_name(b,"autosave_enabled"); json_builder_add_boolean_value(b, s->autosave_enabled);
  json_builder_set_member_name(b,"autosave_interval_sec"); json_builder_add_int_value(b, s->autosave_interval_sec);
  json_builder_end_object(b);
  JsonGenerator *g=json_generator_new(); JsonNode *root=json_builder_get_root(b);
  json_generator_set_root(g,root); gchar *out=json_generator_to_data(g,NULL);
  gboolean ok=g_file_set_contents(SETTINGS_JSON,out,-1,NULL);
  g_free(out); json_node_free(root); g_object_unref(g); g_object_unref(b);
  return ok;
}

void umi_settings_free(UmiSettings *s){
  if(!s) return;
  g_free(s->theme);
  g_free(s->umicc_path);
  g_free(s->uaengine_path);
  g_free(s->ripgrep_path);
  g_free(s);
}

static void on_ok_clicked(GtkButton *b, gpointer user){
  (void)b;
  PrefsCtx *c = (PrefsCtx*)user;
  if(!c || !c->s) return;
  g_free(c->s->theme);
  c->s->theme = g_strdup( gtk_drop_down_get_selected(GTK_DROP_DOWN(c->dd_theme))==1 ? "dark" : "light" );
  c->s->font_size = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(c->spin_font));
  g_free(c->s->umicc_path); c->s->umicc_path = g_strdup( gtk_editable_get_text(GTK_EDITABLE(c->e_umicc)) );
  g_free(c->s->uaengine_path); c->s->uaengine_path = g_strdup( gtk_editable_get_text(GTK_EDITABLE(c->e_ua)) );
  g_free(c->s->ripgrep_path); c->s->ripgrep_path = g_strdup( gtk_editable_get_text(GTK_EDITABLE(c->e_rg)) );
  c->s->autosave_enabled = gtk_check_button_get_active(GTK_CHECK_BUTTON(c->chk_auto));
  c->s->autosave_interval_sec = (guint)gtk_spin_button_get_value(GTK_SPIN_BUTTON(c->spin_auto));
  umi_settings_save(c->s);
}

GtkWidget* umi_prefs_dialog_new(GtkWindow *parent, UmiSettings *in_out){
  UmiSettings *s = in_out ? in_out : defaults();
  PrefsCtx *ctx = g_new0(PrefsCtx,1); ctx->s = s;

  GtkWidget *dlg = gtk_dialog_new();
  if(parent) gtk_window_set_transient_for(GTK_WINDOW(dlg), parent);
  gtk_window_set_title(GTK_WINDOW(dlg), "Preferences");

  GtkWidget *area = gtk_dialog_get_content_area(GTK_DIALOG(dlg));
  GtkWidget *v = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  gtk_box_append(GTK_BOX(area), v);

  /* Theme */
  GtkWidget *theme_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_append(GTK_BOX(theme_box), gtk_label_new("Theme:"));
  GtkWidget *dd = gtk_drop_down_new_from_strings((const char*[]){ "light", "dark", NULL });
  gtk_drop_down_set_selected(GTK_DROP_DOWN(dd), (g_ascii_strcasecmp(s->theme,"dark")==0)?1:0);
  gtk_box_append(GTK_BOX(theme_box), dd);
  gtk_box_append(GTK_BOX(v), theme_box);
  ctx->dd_theme = dd;

  /* Font size */
  GtkWidget *font_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_append(GTK_BOX(font_box), gtk_label_new("Font Size:"));
  GtkWidget *spin = gtk_spin_button_new_with_range(8, 32, 1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), s->font_size);
  gtk_box_append(GTK_BOX(font_box), spin);
  gtk_box_append(GTK_BOX(v), font_box);
  ctx->spin_font = spin;

  /* Paths */
  gtk_box_append(GTK_BOX(v), mk_labeled(&ctx->e_umicc, "umicc:", s->umicc_path));
  gtk_box_append(GTK_BOX(v), mk_labeled(&ctx->e_ua,    "UAEngine:", s->uaengine_path));
  gtk_box_append(GTK_BOX(v), mk_labeled(&ctx->e_rg,    "ripgrep:", s->ripgrep_path));

  /* Autosave */
  GtkWidget *auto_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  GtkWidget *chk = gtk_check_button_new_with_label("Enable autosave");
  gtk_check_button_set_active(GTK_CHECK_BUTTON(chk), s->autosave_enabled);
  gtk_box_append(GTK_BOX(auto_box), chk);
  GtkWidget *spin2 = gtk_spin_button_new_with_range(5, 300, 5);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin2), s->autosave_interval_sec);
  gtk_box_append(GTK_BOX(auto_box), gtk_label_new("Interval (sec):"));
  gtk_box_append(GTK_BOX(auto_box), spin2);
  gtk_box_append(GTK_BOX(v), auto_box);
  ctx->chk_auto = chk; ctx->spin_auto = spin2;

  /* Buttons */
  GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  GtkWidget *ok = gtk_button_new_with_label("OK");
  GtkWidget *cancel = gtk_button_new_with_label("Cancel");
  gtk_box_append(GTK_BOX(btns), ok);
  gtk_box_append(GTK_BOX(btns), cancel);
  gtk_box_append(GTK_BOX(v), btns);

  g_signal_connect(ok, "clicked", G_CALLBACK(on_ok_clicked), ctx);
  g_signal_connect_swapped(cancel, "clicked", G_CALLBACK(gtk_window_destroy), dlg);
  g_signal_connect_swapped(ok, "clicked", G_CALLBACK(gtk_window_destroy), dlg);

  g_object_set_data_full(G_OBJECT(dlg), "ctx", ctx, (GDestroyNotify)g_free);

  gtk_window_present(GTK_WINDOW(dlg));
  return dlg;
}
