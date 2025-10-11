/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/file_index.c
 * PURPOSE: Implementation of a recursive file indexer
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/file_index.h"

static void walk_dir(GPtrArray *out, const char *dir){
  GDir *d = g_dir_open(dir, 0, NULL);
  if(!d) return;
  const gchar *name;
  while((name = g_dir_read_name(d))){
    if(name[0]=='.') continue;
    gchar *path = g_build_filename(dir, name, NULL);
    if(g_file_test(path, G_FILE_TEST_IS_DIR)) walk_dir(out, path);
    else g_ptr_array_add(out, g_strdup(path));
    g_free(path);
  }
  g_dir_close(d);
}

UmiFileIndex *umi_index_build(const char *root){
  UmiFileIndex *i = g_new0(UmiFileIndex,1);
  i->root = g_strdup(root?root:".");
  i->files = g_ptr_array_new_with_free_func(g_free);
  walk_dir(i->files, i->root);
  return i;
}

void umi_index_refresh(UmiFileIndex *idx){
  if(!idx) return;
  g_ptr_array_set_size(idx->files, 0);
  walk_dir(idx->files, idx->root);
}

void umi_index_free(UmiFileIndex *i){
  if(!i) return;
  g_free(i->root);
  g_ptr_array_free(i->files, TRUE);
  g_free(i);
}
