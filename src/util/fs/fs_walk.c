/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/fs_walk.c
 * PURPOSE: Implementation of recursive walk using GDir
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/fs_walk.h"

static void walk_dir(const char *root, gboolean include_hidden, UmiFsVisitCb cb, gpointer user){
  if(!root || !cb) return;
  GDir *d = g_dir_open(root, 0, NULL);
  if(!d) return;
  const gchar *name;
  while((name = g_dir_read_name(d))){
    if(!include_hidden && name[0]=='.') continue;
    gchar *path = g_build_filename(root, name, NULL);
    gboolean is_dir = g_file_test(path, G_FILE_TEST_IS_DIR);
    cb(user, path, is_dir);
    if(is_dir) walk_dir(path, include_hidden, cb, user);
    g_free(path);
  }
  g_dir_close(d);
}

void umi_fs_walk(const char *root, gboolean include_hidden, UmiFsVisitCb cb, gpointer user){
  if(!root || !cb) return;
  if(!g_file_test(root, G_FILE_TEST_IS_DIR)){
    cb(user, root, FALSE);
    return;
  }
  walk_dir(root, include_hidden, cb, user);
}
