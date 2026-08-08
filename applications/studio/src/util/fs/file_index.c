/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/fs/file_index.c
 *
 * PURPOSE:
 *   Maintain a simple in-memory index of files under a root directory.
 *   - Build the index from disk (recursive scan via fs_walk)
 *   - Refresh it (clear + rescan)
 *   - Free all resources
 *
 * NOTES:
 *   - Matches the public API in src/util/fs/include/file_index.h exactly:
 *       UmiFileIndex *umi_index_build(const char *root);
 *       void          umi_index_refresh(UmiFileIndex *idx);
 *       void          umi_index_free(UmiFileIndex *idx);
 *   - Stores g_strdup'd, canonicalized (normalized) file paths in a GPtrArray.
 *   - No GTK dependencies; pure GLib.
 *
 * THREADING:
 *   - Synchronous; call off the GTK main loop if the tree is large.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/
#include "include/file_index.h"
#include "include/fs_walk.h"
#include <string.h>

/*---------------------------------------------------------------------------
 * Private structure for the index. Kept internal to avoid ABI churn.
 *-------------------------------------------------------------------------*/
struct _UmiFileIndex {
    char      *root;     /* Absolute, canonical root directory path      */
    GPtrArray *files;    /* Owned array of g_strdup'd file paths         */
};

/*---------------------------------------------------------------------------
 * Helper: free all file strings and reset the array to empty.
 *-------------------------------------------------------------------------*/
static void
clear_files(UmiFileIndex *idx)
{
    if (!idx || !idx->files) return;
    /* GPtrArray does not have a clear-with-free; remove one by one to free. */
    for (guint i = 0; i < idx->files->len; ++i) {
        g_free(idx->files->pdata[i]);
    }
    g_ptr_array_set_size(idx->files, 0);
}

/*---------------------------------------------------------------------------
 * Walker callback: collect only REGULAR FILES (not directories).
 *-------------------------------------------------------------------------*/
static void
on_visit(const char *path, gboolean is_dir, gpointer user)
{
    UmiFileIndex *idx = (UmiFileIndex *)user;
    if (!idx || !idx->files) return;

    if (is_dir) {
        return;                                         /* ignore directories   */
    }
    /* Own a duplicate of the canonicalized path. */
    g_ptr_array_add(idx->files, g_strdup(path));
}

/*---------------------------------------------------------------------------
 * Stable sort: correctly-typed comparator (no casting warnings).
 *-------------------------------------------------------------------------*/
static gint
cmp_paths(gconstpointer a, gconstpointer b, gpointer user_data)
{
    (void)user_data;
    const char *sa = (const char *)a;
    const char *sb = (const char *)b;
    return g_strcmp0(sa, sb);
}

/*---------------------------------------------------------------------------
 * Public: build a new index from 'root'.
 *-------------------------------------------------------------------------*/
UmiFileIndex *
umi_index_build(const char *root)
{
    if (!root || !*root) return NULL;

    UmiFileIndex *idx = g_new0(UmiFileIndex, 1);
    idx->root  = g_canonicalize_filename(root, NULL);
    idx->files = g_ptr_array_new_with_free_func(g_free);

    /* Walk and collect; include_hidden = FALSE by default. */
    if (!umi_fs_walk(idx->root, FALSE, on_visit, idx)) {
        /* If walking fails, return a valid (empty) index so callers can still
         * inspect 'root' and later attempt refresh().
         */
    }

    /* Sort for deterministic iteration order. */
    g_ptr_array_sort_with_data(idx->files, cmp_paths, NULL);
    return idx;
}

/*---------------------------------------------------------------------------
 * Public: refresh the existing index by clearing and re-walking.
 *-------------------------------------------------------------------------*/
void
umi_index_refresh(UmiFileIndex *idx)
{
    if (!idx) return;

    clear_files(idx);
    (void)umi_fs_walk(idx->root, FALSE, on_visit, idx);
    g_ptr_array_sort_with_data(idx->files, cmp_paths, NULL);
}

/*---------------------------------------------------------------------------
 * Public: free the index and all owned data.
 *-------------------------------------------------------------------------*/
void
umi_index_free(UmiFileIndex *idx)
{
    if (!idx) return;

    clear_files(idx);
    if (idx->files) g_ptr_array_free(idx->files, TRUE);
    g_clear_pointer(&idx->root, g_free);
    g_free(idx);
}

/*---------------------------------------------------------------------------
 * Public helper: expose a read-only view (non-owning) of the paths, for panels
 * that want to render lists without copying.
 *-------------------------------------------------------------------------*/
const char * const *
umi_index_files_real(const UmiFileIndex *idx, guint *out_len)
{
    if (!idx || !idx->files) {
        if (out_len) *out_len = 0;
        return NULL;
    }
    if (out_len) *out_len = idx->files->len;
    return (const char * const *)idx->files->pdata;
}
