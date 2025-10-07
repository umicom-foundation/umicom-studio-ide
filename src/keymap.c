/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/keymap.c
 * PURPOSE: Simple keyboard shortcut installation (GTK4 EventControllerKey)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#include "keymap.h"

typedef struct {
  UmiKeymapCallbacks km;
} KeymapCtx;

static gboolean on_key_pressed(GtkEventControllerKey *c, guint keyval, guint keycode,
                               GdkModifierType state, gpointer user){
  (void)keycode;
  KeymapCtx *ctx = (KeymapCtx*)user;
  if(!ctx) return FALSE;
  gboolean ctrl  = (state & GDK_CONTROL_MASK) != 0;
  gboolean shift = (state & GDK_SHIFT_MASK) != 0;

  if(ctrl && shift && (keyval == GDK_KEY_P)){
    if(ctx->km.palette) ctx->km.palette(ctx->km.user);
    return TRUE;
  }
  if(ctrl && !shift && (keyval == GDK_KEY_s || keyval == GDK_KEY_S)){
    if(ctx->km.save) ctx->km.save(ctx->km.user);
    return TRUE;
  }
  if(ctrl && shift && (keyval == GDK_KEY_S)){
    if(ctx->km.save_as) ctx->km.save_as(ctx->km.user);
    return TRUE;
  }
  if(!ctrl && !shift && (keyval == GDK_KEY_F5)){
    if(ctx->km.run) ctx->km.run(ctx->km.user);
    return TRUE;
  }
  if(shift && (keyval == GDK_KEY_F5)){
    if(ctx->km.stop) ctx->km.stop(ctx->km.user);
    return TRUE;
  }
  if(ctrl && !shift && (keyval == GDK_KEY_f || keyval == GDK_KEY_F)){
    if(ctx->km.focus_search) ctx->km.focus_search(ctx->km.user);
    return TRUE;
  }
  return FALSE;
}

void umi_keymap_install(GtkWindow *win, const UmiKeymapCallbacks *km){
  if(!win || !km) return;
  KeymapCtx *ctx = g_new0(KeymapCtx,1);
  ctx->km = *km;
  GtkEventController *ctl = gtk_event_controller_key_new();
  g_signal_connect(ctl, "key-pressed", G_CALLBACK(on_key_pressed), ctx);
  gtk_widget_add_controller(GTK_WIDGET(win), ctl);
  /* Note: ctx is intentionally leaked for app lifetime simplicity. */
}
