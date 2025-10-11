/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/project_manager.c
 * PURPOSE: Implementation of project manager (step-by-step comments)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/project_manager.h"

UmiProjectManager *umi_project_manager_new(UmiWorkspace *ws, UmiRecent *recent, UmiStatus *status){
  /* We *do not* take ownership of ws/recent/status because other parts of the app
   * also refer to them. We simply remember the pointers for coordinated actions. */
  UmiProjectManager *pm = g_new0(UmiProjectManager,1);
  pm->ws = ws;
  pm->recent = recent;
  pm->status = status;
  pm->index = NULL; /* built on-demand */
  return pm;
}

gboolean umi_project_open(UmiProjectManager *pm, const char *folder, GError **err){
  if(!pm || !folder || !*folder){ g_set_error(err, g_quark_from_string("umicom"), 1, "No folder given"); return FALSE; }
  /* 1) Set the workspace root (this updates configuration on disk). */
  umi_workspace_set_root(pm->ws, folder);
  /* 2) Build the file index so the file tree shows content immediately. */
  if(pm->index) umi_index_free(pm->index);
  pm->index = umi_index_build(folder);
  /* 3) Record into recent list so the welcome screen remembers it. */
  umi_recent_add(pm->recent, folder); umi_recent_save(pm->recent);
  /* 4) Tell the human what happened. */
  if(pm->status){ gchar *msg = g_strdup_printf("Project opened: %s", folder); umi_status_set(pm->status, msg); g_free(msg); }
  return TRUE;
}

void umi_project_refresh_index(UmiProjectManager *pm){
  if(!pm) return;
  if(!pm->index && pm->ws){
    const char *r = umi_workspace_root(pm->ws);
    if(r) pm->index = umi_index_build(r);
  }else if(pm->index){
    umi_index_refresh(pm->index);
  }
}

void umi_project_manager_free(UmiProjectManager *pm){
  if(!pm) return;
  if(pm->index) umi_index_free(pm->index);
  g_free(pm);
}
