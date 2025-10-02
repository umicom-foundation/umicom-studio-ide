/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/argv.c
 * PURPOSE: Implement umi_argv_apply (non-destructive, reads pairs)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "argv.h"
#include "app.h"
#include "theme.h"
#include "workspace.h"

gboolean umi_argv_apply(GtkApplication *app, int *argc, char ***argv){
  if(!app || !argc || !argv || !*argv) return FALSE;
  gboolean any = FALSE;
  for(int i=1;i<*argc;i++){
    const char *a = (*argv)[i];
    if(g_strcmp0(a,"--root")==0 && i+1<*argc){
      const char *dir = (*argv)[++i];
      UmiApp *ua = umi_app_handle(app);
      if(ua && ua->tree){
        /* best-effort: directly manipulate tree via workspace once available */
        (void)dir;
      }
      any = TRUE;
    }else if(g_strcmp0(a,"--open")==0 && i+1<*argc){
      const char *file = (*argv)[++i];
      UmiApp *ua = umi_app_handle(app);
      if(ua && ua->ed){
        GError *e=NULL; umi_editor_open_file(ua->ed, file, &e); if(e) g_error_free(e);
      }
      any = TRUE;
    }else if(g_strcmp0(a,"--theme")==0 && i+1<*argc){
      const char *name = (*argv)[++i];
      UmiApp *ua = umi_app_handle(app);
      if(ua && ua->win) umi_theme_apply(ua->win, name);
      any = TRUE;
    }
  }
  return any;
}
