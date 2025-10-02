/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/env.c
 * PURPOSE: Implementation of multi-line env parsing helpers.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#include "env.h"

gchar **umi_env_from_multiline(const char *env_multiline){
  if(!env_multiline || !*env_multiline) return NULL;
  GPtrArray *a = g_ptr_array_new_with_free_func(g_free);
  const char *p = env_multiline;
  while(*p){
    const char *line_end = strchr(p, '\n');
    gsize len = line_end ? (gsize)(line_end - p) : strlen(p);
    if(len > 0){
      gchar *line = g_strndup(p, len);
      gchar *trim = g_strstrip(line);
      if(*trim && strchr(trim, '=')){
        g_ptr_array_add(a, g_strdup(trim));
      }
      g_free(line);
    }
    if(!line_end) break;
    p = line_end + 1;
  }
  g_ptr_array_add(a, NULL);
  return (gchar**)g_ptr_array_free(a, FALSE); /* transfer */
}
