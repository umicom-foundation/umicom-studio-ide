/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/fs/fs_walk.c
 * PURPOSE: Implementation of recursive walk using GDir
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "fs_walk.h"               /* Public API */

#include <string.h>                        /* strcmp (for optional sorting) */

/* Internal helper: optionally collect names to sort for stable output. */
static void walk_dir(const char *root, gboolean include_hidden, UmiFsVisitCb cb, gpointer user){
  if(!root || !cb)                        /* Validate inputs: root and callback are required. */
    return;

  GDir *d = g_dir_open(root, 0, NULL);    /* Try opening directory (NULL error for simplicity). */
  if(!d)                                  /* If we failed (permissions/ENOENT), stop descending. */
    return;

  /* Read all names to allow stable sorting; avoids OS-dependent order variance. */
  GPtrArray *names = g_ptr_array_new_with_free_func(g_free); /* Store strdup'd entry names. */
  const gchar *name;                       /* Current directory entry pointer (non-owned). */
  while((name = g_dir_read_name(d))){      /* Iterate until NULL (no more entries). */
    if(!include_hidden && name[0]=='.')    /* Skip hidden names when requested. */
      continue;
    g_ptr_array_add(names, g_strdup(name));/* Take a copy for later processing. */
  }
  g_dir_close(d);                          /* Close the directory now that we buffered names. */

  g_ptr_array_sort_with_data(              /* Stable order for predictable builds/tests. */
    names,
    (GCompareDataFunc)g_strcmp0,           /* Compare function for strings. */
    NULL                                   /* No extra user data needed. */
  );

  /* Now visit each name (sorted). */
  for(guint i=0;i<names->len;i++){         /* Loop through all collected names. */
    const char *n = (const char*)names->pdata[i]; /* Borrowed pointer into array's element. */
    gchar *path = g_build_filename(root, n, NULL);/* Build full path with platform separator. */
    gboolean is_dir = g_file_test(path, G_FILE_TEST_IS_DIR); /* Cheap stat to see if it's a folder. */
    cb(user, path, is_dir);                /* Notify caller about this entry. */
    if(is_dir){                            /* If it's a directory... */
      /* Basic symlink loop avoidance: skip if it's a symlink to a dir (don’t recurse). */
      if(!g_file_test(path, G_FILE_TEST_IS_SYMLINK))
        walk_dir(path, include_hidden, cb, user); /* Recurse into subdir. */
    }
    g_free(path);                          /* Free the path we constructed. */
  }

  g_ptr_array_free(names, TRUE);           /* Free array + contained strdup'd names. */
}

/* Public entry point. */
void umi_fs_walk(const char *root, gboolean include_hidden, UmiFsVisitCb cb, gpointer user){
  if(!root || !cb)                         /* Nothing useful to do if inputs are missing. */
    return;

  if(!g_file_test(root, G_FILE_TEST_IS_DIR)){ /* If root is not a dir... */
    cb(user, root, FALSE);                 /* ...treat it as a single file visit. */
    return;
  }

  walk_dir(root, include_hidden, cb, user); /* Otherwise, descend recursively. */
}

/* Correct-typed adapter that matches GCompareDataFunc signature for strings. */
static gint cmp_cstrings(gconstpointer a, gconstpointer b, gpointer user_data) {
    (void)user_data;                              // Unused.
    const char *sa = a;                           // Input 'a' as C string.
    const char *sb = b;                           // Input 'b' as C string.
    return g_strcmp0(sa, sb);                     // Use GLib-safe strcmp.
}
/* Sort a list of paths lexicographically (in-place). */
void umi_fs_sort_paths(GPtrArray *arr){
  if(!arr)                                   /* Safe on NULL. */
    return;
  g_ptr_array_sort_with_data(                /* Stable order for reproducible builds/tests. */
    arr,
    (GCompareDataFunc)cmp_cstrings,          /* Compare function for strings. */
    NULL                                     /* No extra user data needed. */
  );
}
/* End of src/util/fs/fs_walk.c */
