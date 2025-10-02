/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util_path.c
 * PURPOSE: Implementation of path helpers using GLib
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "util_path.h"

static gchar *norm_only(const char *p){
  if(!p||!*p) return g_strdup(".");
  gchar **parts = g_strsplit(p, G_DIR_SEPARATOR_S, -1);
  GPtrArray *out = g_ptr_array_new_with_free_func(g_free);
  for(gchar **i=parts; i && *i; ++i){
    const char *seg = *i;
    if(!seg || !*seg || g_str_equal(seg,".")) continue;
    if(g_str_equal(seg,"..")){
      if(out->len>0) g_ptr_array_remove_index(out, out->len-1);
      else g_ptr_array_add(out, g_strdup(".."));
    }else{
      g_ptr_array_add(out, g_strdup(seg));
    }
  }
  gchar *joined = g_strjoinv(G_DIR_SEPARATOR_S, (gchar**)out->pdata);
  g_ptr_array_free(out, TRUE);
  g_strfreev(parts);
#ifdef G_OS_WIN32
  /* Keep drive letters/UNC out of scope for brevity; callers should supply absolute 'base' */
#endif
  return joined;
}

gchar *umi_path_normalize(const char *path, const char *base){
  if(!path||!*path) return g_strdup(".");
  if(g_path_is_absolute(path)) return norm_only(path);
  if(base && *base){
    gchar *abs = g_build_filename(base, path, NULL);
    gchar *n = norm_only(abs);
    g_free(abs);
    return n;
  }
  return norm_only(path);
}

gboolean umi_path_is_subpath(const char *parent, const char *child){
  if(!parent||!child) return FALSE;
  gchar *p = umi_path_normalize(parent, NULL);
  gchar *c = umi_path_normalize(child,  NULL);
  gboolean ok = g_str_has_prefix(c, p) &&
                (c[g_utf8_strlen(p,-1)]==G_DIR_SEPARATOR || c[g_utf8_strlen(p,-1)]=='\0');
  g_free(p); g_free(c);
  return ok;
}

gchar *umi_path_make_relative(const char *base, const char *target){
  if(!base||!target) return g_strdup(target?target:"");
  gchar *b = umi_path_normalize(base,NULL);
  gchar *t = umi_path_normalize(target,NULL);
  if(!g_str_has_prefix(t,b)){
    g_free(b); return t; /* cannot relativize */
  }
  gsize off = strlen(b);
  while(t[off]==G_DIR_SEPARATOR) off++;
  gchar *rel = g_strdup(t+off);
  g_free(b); g_free(t);
  return rel;
}
