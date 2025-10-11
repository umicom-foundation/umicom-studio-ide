/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/watcher_recursive.c
 * PURPOSE: Implementation of recursive watcher using multiple GFileMonitor
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/watcher_recursive.h"

typedef struct {
  GFile *dir;
  GFileMonitor *mon;
} Mon;

struct _UmiWatchRec {
  gchar *root;
  GPtrArray *mons; /* Mon* */
  UmiWatchEventCb cb;
  gpointer user;
};

static void mon_free(Mon *m){
  if(!m) return;
  if(m->mon) g_file_monitor_cancel(m->mon);
  g_clear_object(&m->mon);
  g_clear_object(&m->dir);
  g_free(m);
}

static void on_changed(GFileMonitor *m, GFile *file, GFile *other, GFileMonitorEvent e, gpointer u){
  (void)m;(void)other;(void)e;
  UmiWatchRec *w = (UmiWatchRec*)u;
  if(!w || !w->cb || !file) return;
  gchar *p = g_file_get_path(file);
  w->cb(w->user, p?p:"");
  g_free(p);
}

static void scan_add(UmiWatchRec *w, const char *dir){
  GDir *d = g_dir_open(dir, 0, NULL);
  if(!d) return;
  const gchar *name;
  while((name = g_dir_read_name(d))){
    if(name[0]=='.') continue;
    gchar *path = g_build_filename(dir, name, NULL);
    if(g_file_test(path, G_FILE_TEST_IS_DIR)){
      /* monitor this dir */
      Mon *m = g_new0(Mon,1);
      m->dir = g_file_new_for_path(path);
      GError *e=NULL;
      m->mon = g_file_monitor_directory(m->dir, G_FILE_MONITOR_NONE, NULL, &e);
      if(m->mon){
        g_signal_connect(m->mon, "changed", G_CALLBACK(on_changed), w);
        g_ptr_array_add(w->mons, m);
      }else{
        if(e){ g_warning("watcher-rec: %s", e->message); g_error_free(e); }
        mon_free(m);
      }
      /* Recurse */
      scan_add(w, path);
    }
    g_free(path);
  }
  g_dir_close(d);
}

static void stop_all(UmiWatchRec *w){
  if(!w) return;
  for(guint i=0;i<w->mons->len;i++) mon_free((Mon*)w->mons->pdata[i]);
  g_ptr_array_set_size(w->mons, 0);
}

static void restart(UmiWatchRec *w){
  if(!w || !w->root) return;
  stop_all(w);
  /* Always monitor the root itself */
  Mon *m = g_new0(Mon,1);
  m->dir = g_file_new_for_path(w->root);
  GError *e=NULL; m->mon = g_file_monitor_directory(m->dir, G_FILE_MONITOR_NONE, NULL, &e);
  if(m->mon){
    g_signal_connect(m->mon, "changed", G_CALLBACK(on_changed), w);
    g_ptr_array_add(w->mons, m);
  }else{
    if(e){ g_warning("watcher-rec: root %s", e->message); g_error_free(e); }
    mon_free(m);
  }
  scan_add(w, w->root);
}

UmiWatchRec *umi_watchrec_new(const char *root, UmiWatchEventCb cb, gpointer user){
  UmiWatchRec *w = g_new0(UmiWatchRec,1);
  w->mons = g_ptr_array_new();
  w->cb = cb; w->user = user;
  w->root = g_strdup(root?root:".");
  restart(w);
  return w;
}

void umi_watchrec_set_root(UmiWatchRec *w, const char *root){
  if(!w) return;
  g_free(w->root);
  w->root = g_strdup(root?root:".");
  restart(w);
}

void umi_watchrec_free(UmiWatchRec *w){
  if(!w) return;
  stop_all(w);
  g_ptr_array_free(w->mons, TRUE);
  g_free(w->root);
  g_free(w);
}
