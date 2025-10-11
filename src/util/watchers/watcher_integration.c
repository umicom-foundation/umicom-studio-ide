/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/watcher_integration.c
 * PURPOSE: Implementation of watcher glue
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/watcher_integration.h"

static void on_evt(gpointer u, const char *path){
  (void)path;
  UmiWatchGlue *g = (UmiWatchGlue*)u;
  if(g && g->tree) umi_file_tree_refresh(g->tree);
}

UmiWatchGlue *umi_watchglue_start(UmiWorkspace *ws, UmiFileTree *tree){
  if(!ws || !tree) return NULL;
  UmiWatchGlue *g = g_new0(UmiWatchGlue,1);
  g->ws = ws; g->tree = tree;
  const char *root = umi_workspace_root(ws);
  g->rec = umi_watchrec_new(root?root:".", on_evt, g);
  return g;
}

void umi_watchglue_stop(UmiWatchGlue *g){
  if(!g) return;
  if(g->rec) umi_watchrec_free(g->rec);
  g_free(g);
}
