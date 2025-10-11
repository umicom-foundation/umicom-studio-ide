/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/timestamp.c
 * PURPOSE: Implementation of timestamp helpers
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/timestamp.h"

gchar *umi_now_iso8601(void){
  GDateTime *dt = g_date_time_new_now_local();
  gchar *s = g_date_time_format_iso8601(dt);
  g_date_time_unref(dt);
  return s;
}

gchar *umi_iso8601_from_time(gint64 secs){
  GDateTime *dt = g_date_time_new_from_unix_local(secs);
  gchar *s = g_date_time_format_iso8601(dt);
  g_date_time_unref(dt);
  return s;
}
