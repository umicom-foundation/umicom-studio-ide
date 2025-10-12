/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/fs/file_index.c
 * PURPOSE: Recursive file indexer
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/file_index.h"           /* Public header for UmiFileIndex */

#include <glib/gstdio.h>                  /* g_file_test, etc. */
#include <string.h>                       /* strcmp for sorting */

/* Internal: recursively walk 'dir' and append file paths to idx->files. */
static void add_dir(UmiFileIndex *idx, const char *dir){
  if(!idx || !dir)                                   /* Defensive: nothing to do without args. */
    return;

  GDir *d = g_dir_open(dir, 0, NULL);                /* Try opening directory. */
  if(!d)                                             /* If can't open (permission/ENOENT), stop. */
    return;

  /* Collect entries to sort for deterministic order. */
  GPtrArray *names = g_ptr_array_new_with_free_func(g_free); /* strdup'd names. */
  const gchar *name;                                  /* Non-owned pointer per iteration. */
  while((name = g_dir_read_name(d))){                 /* Iterate until NULL. */
    if(name[0]=='.')                                  /* Skip hidden entries by policy. */
      continue;
    g_ptr_array_add(names, g_strdup(name));           /* Keep a copy for later sorting. */
  }
  g_dir_close(d);                                     /* Done reading directory. */

  g_ptr_array_sort_with_data(                         /* Stable order for reproducible builds/tests. */
    names, (GCompareDataFunc)g_strcmp0, NULL);

  /* Visit each entry (sorted). */
  for(guint i=0;i<names->len;i++){
    const char *n = (const char*)names->pdata[i];     /* Borrow current name. */
    gchar *path = g_build_filename(dir, n, NULL);     /* Build full child path. */

    gboolean is_dir = g_file_test(path, G_FILE_TEST_IS_DIR);        /* Check directory-ness. */
    gboolean is_link = g_file_test(path, G_FILE_TEST_IS_SYMLINK);   /* Symlink guard. */

    if(is_dir){
      if(!is_link)                                    /* Avoid simple symlink loops into dirs. */
        add_dir(idx, path);                           /* Recurse into subdir. */
    }else{
      /* Canonicalize files; store absolute-like path for consistency. */
      gchar *canon = g_canonicalize_filename(path, idx->root); /* Root-anchored canonical path. */
      g_ptr_array_add(idx->files, canon);             /* Takes ownership of 'canon'. */
    }

    g_free(path);                                     /* Free child path string. */
  }

  g_ptr_array_free(names, TRUE);                      /* Free the list of names. */
}

/* Build a new file index from 'root'. */
UmiFileIndex *umi_index_build(const char *root){
  if(!root || !*root)                                  /* Require a root path. */
    return NULL;

  /* Allocate and initialize the index object. */
  UmiFileIndex *idx = g_new0(UmiFileIndex, 1);         /* Zeroed struct allocation. */
  idx->files = g_ptr_array_new_with_free_func(g_free); /* Each element is a g_strdup'd string. */
  idx->root  = g_canonicalize_filename(root, NULL);    /* Canonical root for stable joins. */

  if(!g_file_test(idx->root, G_FILE_TEST_IS_DIR)){     /* If root isn't a directory... */
    /* If it's a file, index exactly that one. */
    gchar *canon = g_canonicalize_filename(idx->root, NULL); /* Canonicalize file path. */
    g_ptr_array_add(idx->files, canon);                /* Add the file to index. */
    return idx;                                        /* Return single-file index. */
  }

  add_dir(idx, idx->root);                             /* Recursively collect files. */

  /* Sort final list lexicographically for stability (in case recursion order drifted). */
  g_ptr_array_sort_with_data(idx->files, (GCompareDataFunc)g_strcmp0, NULL);

  return idx;                                          /* Return the built index. */
}

/* Clear and rebuild the file list for an existing index. */
void umi_index_refresh(UmiFileIndex *idx){
  if(!idx)                                             /* Safe on NULL. */
    return;

  /* Reset file list to empty (freeing all strings). */
  for(guint i=0;i<idx->files->len;i++)                 /* Free any leftover elements (paranoia). */
    g_free(idx->files->pdata[i]);
  g_ptr_array_set_size(idx->files, 0);                 /* Logical length = 0; storage kept. */

  /* Re-walk from stored root. */
  if(!g_file_test(idx->root, G_FILE_TEST_IS_DIR)){     /* If root is a file... */
    gchar *canon = g_canonicalize_filename(idx->root, NULL); /* Canonicalize and store. */
    g_ptr_array_add(idx->files, canon);                /* Add the file. */
    return;
  }

  add_dir(idx, idx->root);                             /* Otherwise, rescan the tree. */

  /* Keep results sorted. */
  g_ptr_array_sort_with_data(idx->files, (GCompareDataFunc)g_strcmp0, NULL);
}

/* Release all resources held by the index. */
void umi_index_free(UmiFileIndex *idx){
  if(!idx)                                             /* Allow free(NULL). */
    return;
  if(idx->files){                                      /* If list exists... */
    g_ptr_array_free(idx->files, TRUE);                /* Free list and its strings. */
  }
  g_free(idx->root);                                   /* Free canonical root string. */
  g_free(idx);                                         /* Free the struct itself. */
}
