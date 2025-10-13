/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/fs/include/file_index.h
 *
 * PURPOSE:
 *   Public API for a tiny in-memory file index used by panels that need fast
 *   access to "all files under a root directory". The index stores immutable
 *   (g_strdup'ed) absolute, canonicalized paths in a GLib GPtrArray.
 *
 *  API (stable):
 *    - UmiFileIndex *umi_index_build(const char *root);
 *        Build a new index from disk by walking 'root' recursively.
 *        Returns a heap-allocated handle you must later free via umi_index_free().
 *
 *    - void umi_index_refresh(UmiFileIndex *idx);
 *        Clear and re-scan the index's files from disk using the same root.
 *
 *    - void umi_index_free(UmiFileIndex *idx);
 *        Release all resources (the array and the strings inside).
 *
 * NOTES:
 *   - The index contains only regular files (not directories). If you need both,
 *     adapt the callback in file_index.c (search for 'on_visit').
 *   - No GTK dependencies; pure GLib.
 *   - Threading: synchronous; scanning large trees should be dispatched off the UI thread.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FILE_INDEX_H
#define UMICOM_FILE_INDEX_H

#include <glib.h>

G_BEGIN_DECLS

/*---------------------------------------------------------------------------
 * Forward-declared opaque handle.
 * Internals (root + GPtrArray of char*) live in the C file to keep the header
 * minimal and stable.
 *-------------------------------------------------------------------------*/
typedef struct _UmiFileIndex UmiFileIndex;

/*---------------------------------------------------------------------------
 * Build a fresh index from 'root' (recursive).
 *
 * RETURNS:
 *   Newly-allocated UmiFileIndex* or NULL if 'root' is invalid/unreachable.
 *-------------------------------------------------------------------------*/
UmiFileIndex *umi_index_build(const char *root);

/*---------------------------------------------------------------------------
 * Clear and re-scan files from disk into 'idx' using its original root path.
 * - Safe to call multiple times.
 * - No-op if 'idx' is NULL.
 *-------------------------------------------------------------------------*/
void          umi_index_refresh(UmiFileIndex *idx);

/*---------------------------------------------------------------------------
 * Release all resources of the index. Safe on NULL.
 *-------------------------------------------------------------------------*/
void          umi_index_free   (UmiFileIndex *idx);

/*---------------------------------------------------------------------------
 * Convenience: fetch read-only slice view of the internal GPtrArray so callers
 * can iterate without copying. This is non-owning; do not free/modify entries.
 * Provided as a helper for panels and tests.
 *-------------------------------------------------------------------------*/
static inline const char * const *umi_index_files(const UmiFileIndex *idx, guint *out_len)
{
    /* Implementation is in the C file; here we just provide the signature to
     * consumers that include the header. The actual function is declared below
     * as a normal symbol. This inline stub is intentionally empty to keep the
     * header self-contained on compilers that accept 'extern inline' quirks.
     */
    (void)idx; (void)out_len;
    return NULL;
}

/* Real symbol (defined in file_index.c). */
const char * const *umi_index_files_real(const UmiFileIndex *idx, guint *out_len);

/* Macro to route to the real function while keeping the simple name. */
#define umi_index_files(idx, out_len) umi_index_files_real((idx), (out_len))

G_END_DECLS

#endif /* UMICOM_FILE_INDEX_H */
