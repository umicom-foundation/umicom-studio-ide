/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/path_watcher.c
 * PURPOSE: Implementation using a single GFileMonitor (non-recursive)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/path_watcher.h"

struct _UmiPathWatcher {
  GFile *root;
  GFileMonitor *mon;
  UmiWatchRefreshCb cb;
  gpointer user;
};

static void on_changed(GFileMonitor *m, GFile *file, GFile *other, GFileMonitorEvent e, gpointer user){
  (void)m;(void)file;(void)other;(void)e;
  UmiPathWatcher *w = (UmiPathWatcher*)user;
  if(w && w->cb) w->cb(w->user);
}

static void stop(UmiPathWatcher *w){
  if(!w) return;
  if(w->mon){ g_file_monitor_cancel(w->mon); g_clear_object(&w->mon); }
  g_clear_object(&w->root);
}

static void start(UmiPathWatcher *w, const char *root){
  if(!w) return;
  stop(w);
  if(!root||!*root) return;
  w->root = g_file_new_for_path(root);
  GError *e=NULL;
  w->mon = g_file_monitor_directory(w->root, G_FILE_MONITOR_NONE, NULL, &e);
  if(w->mon){
    g_signal_connect(w->mon, "changed", G_CALLBACK(on_changed), w);
  }else if(e){
    g_warning("watcher: %s", e->message);
    g_error_free(e);
  }
}

UmiPathWatcher *umi_watch_new(const char *root, UmiWatchRefreshCb cb, gpointer user){
  UmiPathWatcher *w = g_new0(UmiPathWatcher,1);
  w->cb = cb; w->user = user;
  start(w, root);
  return w;
}

void umi_watch_set_root(UmiPathWatcher *w, const char *root){ start(w, root); }

void umi_watch_free(UmiPathWatcher *w){
  if(!w) return;
  stop(w);
  g_free(w);
}
