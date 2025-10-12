/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/fs/util_path.c
 * PURPOSE: Implementation of path helpers using GLib
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "util_path.h"        /* Our public declarations */

/* --- Internal helper: collapse "." and ".." without touching symlinks --- */
static gchar *norm_only(const char *p){
  if(!p || !*p)                      /* If NULL or empty... */
    return g_strdup(".");            /* ...treat as "." (current directory). */

  /* Split by the platform separator ("/" on Unix, "\" on Windows). */
  gchar **parts = g_strsplit(p, G_DIR_SEPARATOR_S, -1); /* Vector of segments (NULL-terminated). */
  GPtrArray *out = g_ptr_array_new_with_free_func(g_free); /* Dynamic array of normalized parts. */

  /* Process each segment one by one. */
  for(gchar **i = parts; i && *i; ++i){          /* Iterate over all segments. */
    const char *seg = *i;                        /* Current segment text. */
    if(!seg || !*seg || g_str_equal(seg, ".")){  /* Skip empty or "." segments. */
      continue;                                  /* No-op for "."; empty from leading/trailing "/" too. */
    }
    if(g_str_equal(seg, "..")){                  /* Parent directory indicator. */
      if(out->len > 0){                          /* If we have something to back up over... */
        g_ptr_array_remove_index(out, out->len - 1); /* Pop the last segment. (Free func cleans it.) */
      }else{
        g_ptr_array_add(out, g_strdup(".."));    /* If at the beginning, keep ".." literally. */
      }
    }else{
      g_ptr_array_add(out, g_strdup(seg));       /* Regular name: keep as-is. */
    }
  }

  /* Join back with the platform separator. */
  gchar *joined = g_strjoinv(G_DIR_SEPARATOR_S, (gchar**)out->pdata); /* New allocated string. */
  g_ptr_array_free(out, TRUE);       /* Free array + contained strings (already copied into joined). */
  g_strfreev(parts);                  /* Free the split vector. */

#ifdef G_OS_WIN32
  /* NOTE:
     We purposely do not attempt to repair drive letters or UNC prefixes here.
     Callers that care about absolute semantics should pass a proper 'base'
     to umi_path_normalize() so the result becomes well-formed. */
#endif

  return joined;                      /* Normalized (segment-wise) path. */
}

/* Normalize possibly-relative 'path' against 'base', then collapse segments. */
gchar *umi_path_normalize(const char *path, const char *base){
  if(!path || !*path)                /* NULL or empty path? */
    return g_strdup(".");            /* -> "." */

  if(g_path_is_absolute(path))       /* Already absolute? */
    return norm_only(path);          /* Collapse segments and return. */

  if(base && *base){                 /* Have a base to resolve against? */
    gchar *abs = g_build_filename(base, path, NULL); /* Join base + path (adds sep if needed). */
    gchar *n   = norm_only(abs);     /* Collapse segments on the joined string. */
    g_free(abs);                     /* Free the intermediate joined path. */
    return n;                        /* Return normalized absolute path. */
  }
  return norm_only(path);            /* No base: normalize the relative path textually. */
}

/* TRUE if 'child' lies within 'parent' (after normalization). */
gboolean umi_path_is_subpath(const char *parent, const char *child){
  if(!parent || !child)              /* Defensive programming: NULL inputs => not a subpath. */
    return FALSE;

  gchar *p = umi_path_normalize(parent, NULL);   /* Normalize parent to a clean form. */
  gchar *c = umi_path_normalize(child,  NULL);   /* Normalize child likewise.   */

  /* Check prefix with a segment boundary:
     - Must start with 'p'
     - Next byte in 'c' at position strlen(p) is either separator or end (exact match). */
  gsize plen = strlen(p);            /* BYTE length, not UTF-8 char count (critical for indexing!). */
  gboolean has = g_str_has_prefix(c, p);        /* Quick prefix check. */
  gboolean boundary = has && (c[plen] == '\0' || c[plen] == G_DIR_SEPARATOR); /* Boundary OK? */

  gboolean ok = has && boundary;     /* Final decision. */
  g_free(p);                         /* Free temp strings. */
  g_free(c);
  return ok;                         /* TRUE if child is within (or equal to) parent. */
}

/* Make 'target' relative to 'base' if possible; else return a copy of 'target'. */
gchar *umi_path_make_relative(const char *base, const char *target){
  if(!base || !target)               /* NULL inputs: best effort. */
    return g_strdup(target ? target : ""); /* Return copy of target or empty string. */

  gchar *b = umi_path_normalize(base,   NULL);  /* Normalize base.   */
  gchar *t = umi_path_normalize(target, NULL);  /* Normalize target. */

  gsize blen = strlen(b);            /* BYTE length of base (for boundary checks). */
  gboolean has = g_str_has_prefix(t, b); /* Prefix? */
  gboolean boundary = has && (t[blen] == '\0' || t[blen] == G_DIR_SEPARATOR); /* Boundary OK? */
  if(!boundary){
    g_free(b);                       /* Cannot relativize safely -> return normalized target. */
    return t;
  }

  /* Skip any leading separators after the base to form the relative view. */
  gsize off = blen;                  /* Start immediately after base prefix. */
  while(t[off] == G_DIR_SEPARATOR)   /* Consume duplicate separators if present. */
    off++;

  gchar *rel = g_strdup(t + off);    /* Allocated copy of the relative suffix. */
  g_free(b);                         /* Free temporaries. */
  g_free(t);
  return rel;                        /* Caller must g_free(). */
}
