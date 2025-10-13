/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/fs/fs_walk.c
 *
 * PURPOSE:
 *   Implementation of a deterministic, GLib-based recursive directory walker.
 *   The walker is intentionally tiny and avoids heavy allocations:
 *     - We use GDir to iterate entries.
 *     - We normalize (canonicalize) paths once per visited node.
 *     - We collect directory entries into a GPtrArray, sort them with a
 *       correctly-typed comparator (no function-pointer casting), then iterate.
 *
 * WHY NOT GFileEnumerator?
 *   - It is great, but for our use case GDir keeps dependencies minimal and
 *     avoids async bookkeeping we do not need.
 *
 * WARNING CLEANUP:
 *   - We DO NOT cast g_strcmp0 to GCompareDataFunc (which triggers -Wcast-function-type).
 *     Instead, we provide a thin, correctly-typed adapter (see cmp_names()).
 *
 * THREADING:
 *   - Synchronous; consider using a worker thread when scanning huge trees.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/
#include "include/fs_walk.h"
#include <string.h>

/*---------------------------------------------------------------------------
 * Local, correctly-typed name comparator for GPtrArray sorting.
 *  - 'a' and 'b' are char* elements from the array.
 *  - Returns strcmp-like ordering using g_strcmp0.
 *-------------------------------------------------------------------------*/
static gint
cmp_names(gconstpointer a, gconstpointer b, gpointer user_data)
{
    (void)user_data;                          /* comparator doesn't need it   */
    const char *sa = (const char *)a;
    const char *sb = (const char *)b;
    return g_strcmp0(sa, sb);                 /* GLib-safe string compare     */
}

/*---------------------------------------------------------------------------
 * Decide if an entry (leaf name) should be considered "hidden".
 * On POSIX, entries starting with '.' are treated as hidden. On Windows we
 * keep the same convention for consistency inside the IDE.
 *-------------------------------------------------------------------------*/
static gboolean
is_hidden_name(const char *leaf)
{
    return (leaf && leaf[0] == '.');
}

/*---------------------------------------------------------------------------
 * Build an absolute, canonicalized path for 'parent' + 'leaf'.
 * Uses g_build_filename + g_canonicalize_filename to ensure stable form.
 *-------------------------------------------------------------------------*/
static char *
join_canonical(const char *parent, const char *leaf)
{
    char *p = g_build_filename(parent, leaf, NULL);    /* join with separator   */
    char *c = g_canonicalize_filename(p, NULL);        /* canonicalize absolute */
    g_free(p);
    return c;                                          /* caller must g_free()  */
}

/*---------------------------------------------------------------------------
 * Core recursive walker.
 *  - Gathers entries, sorts deterministically, visits (dir first to match many
 *    tools' expectations), then dives into subdirectories.
 *-------------------------------------------------------------------------*/
static void
walk_dir(const char *root, gboolean include_hidden, UmiFsVisitCb cb, gpointer user)
{
    /* Open the directory; bail if not readable. */
    GError *err = NULL;
    GDir   *dir = g_dir_open(root, 0, &err);
    if (!dir) {
        if (err) g_error_free(err);
        return;
    }

    /* Collect child names first for deterministic ordering. */
    GPtrArray *names = g_ptr_array_new_with_free_func(g_free);
    for (const char *name = g_dir_read_name(dir); name; name = g_dir_read_name(dir)) {
        if (!include_hidden && is_hidden_name(name)) {
            continue;                                    /* skip hidden entries  */
        }
        g_ptr_array_add(names, g_strdup(name));          /* own the copied name  */
    }
    g_dir_close(dir);

    /* Sort using our correctly-typed comparator (no casting). */
    g_ptr_array_sort_with_data(names, cmp_names, NULL);

    /* First pass: visit directories (so parents precede children). */
    for (guint i = 0; i < names->len; ++i) {
        const char *leaf = (const char *)names->pdata[i];
        char *full = join_canonical(root, leaf);
        if (g_file_test(full, G_FILE_TEST_IS_DIR)) {
            if (cb) cb(full, TRUE, user);                /* notify: directory    */
        }
        g_free(full);
    }

    /* Second pass: visit files, then recurse into directories. */
    for (guint i = 0; i < names->len; ++i) {
        const char *leaf = (const char *)names->pdata[i];
        char *full = join_canonical(root, leaf);
        if (g_file_test(full, G_FILE_TEST_IS_DIR)) {
            /* Recurse after parent dir was announced in the first pass. */
            walk_dir(full, include_hidden, cb, user);
        } else {
            if (cb) cb(full, FALSE, user);               /* notify: regular file */
        }
        g_free(full);
    }

    g_ptr_array_free(names, TRUE);                       /* free names + strings */
}

/*---------------------------------------------------------------------------
 * Public entry: umi_fs_walk()
 *  - Validates root, normalizes it, announces the root directory itself, then
 *    walks its content recursively.
 *-------------------------------------------------------------------------*/
gboolean
umi_fs_walk(const char *root, gboolean include_hidden,
            UmiFsVisitCb cb, gpointer user)
{
    if (!root || !*root) {
        return FALSE;                                    /* invalid argument     */
    }

    /* Normalize to an absolute, canonical path. */
    char *canon = g_canonicalize_filename(root, NULL);
    if (!g_file_test(canon, G_FILE_TEST_IS_DIR)) {
        g_free(canon);
        return FALSE;                                    /* missing/unreadable   */
    }

    /* Announce root dir to the callback for symmetry. */
    if (cb) cb(canon, TRUE, user);

    /* Recurse into children. */
    walk_dir(canon, include_hidden, cb, user);

    g_free(canon);
    return TRUE;
}
