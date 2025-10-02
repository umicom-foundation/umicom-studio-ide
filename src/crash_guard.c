/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/crash_guard.c
 * PURPOSE: Implementation of crash guard log writer
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "crash_guard.h"
#include <stdio.h>

static GLogWriterOutput writer(GLogLevelFlags level, const GLogField *fields, gsize n_fields, gpointer user_data){
  (void)user_data;
  const char *msg = NULL, *dom = NULL;
  for(gsize i=0;i<n_fields;i++){
    if(g_strcmp0(fields[i].key, "MESSAGE")==0) msg = (const char*)fields[i].value;
    else if(g_strcmp0(fields[i].key, "GLIB_DOMAIN")==0) dom = (const char*)fields[i].value;
  }
  if(!msg) msg = "";
  fprintf(stderr, "[GLib:%s] %s\n", dom?dom:"", msg);
  /* Tell GLib we handled it and it should continue (no abort) */
  return G_LOG_WRITER_HANDLED;
}

void umi_crash_guard_install(void){
  g_log_set_writer_func(writer, NULL, NULL);
}
