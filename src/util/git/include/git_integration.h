/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/git/include/git_integration.h
 * PURPOSE: Public API for simple Git helpers used by the IDE
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#pragma once
#ifndef UMICOM_GIT_INTEGRATION_H
#define UMICOM_GIT_INTEGRATION_H

#include <glib.h>   /* GError, gchar, gboolean, NULL, g_free */

G_BEGIN_DECLS

/**
 * umi_git_status:
 * @cwd:   Optional working directory to run git in (NULL = current process cwd)
 * @error: (out) return location for a GError*, or NULL to ignore
 *
 * Runs: `git status --porcelain=v1`
 * Returns a newly-allocated string with the raw porcelain output on success.
 * The caller must free it with g_free(). Returns NULL on error and sets @error.
 */
gchar   *umi_git_status  (const char *cwd, GError **error);

/**
 * umi_git_add_all:
 * @cwd:   Optional working directory (NULL = process cwd)
 * @error: (out) error on failure, NULL to ignore
 *
 * Runs: `git add -A`
 * Returns TRUE on exit-code 0, FALSE otherwise (setting @error).
 */
gboolean umi_git_add_all (const char *cwd, GError **error);

/**
 * umi_git_commit:
 * @cwd:     Optional working directory
 * @message: Commit message (must be non-NULL, non-empty)
 * @error:   (out) error on failure, NULL to ignore
 *
 * Runs: `git commit -m <message>`
 * Returns TRUE on exit-code 0, FALSE otherwise (setting @error).
 */
gboolean umi_git_commit  (const char *cwd, const char *message, GError **error);

G_END_DECLS

#endif /* UMICOM_GIT_INTEGRATION_H */
/* End of src/util/git/include/git_integration.h */