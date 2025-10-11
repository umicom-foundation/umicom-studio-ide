/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/git/include/git_integration.h
 * PURPOSE: Minimal Git helpers used by UI/actions (no libgit2 dependency).
 * NOTE: We intentionally shell out to `git` via GLib to avoid heavy deps.
 *---------------------------------------------------------------------------*/
#pragma once

#ifndef UMICOM_GIT_INTEGRATION_H
#define UMICOM_GIT_INTEGRATION_H

#include <gio/gio.h>
#include <glib.h>

/* Run `git status --porcelain` in `dir` and return stdout as newly-allocated
 * string (UTF-8). On error returns NULL and sets GError. */
char *umi_git_status (const char *dir, GError **err);

/* Run `git add -A` in `dir`. Returns TRUE on success, FALSE on error (GError). */
gboolean umi_git_add_all (const char *dir, GError **err);

/* Run `git commit -m <message>` in `dir`. Returns TRUE on success. */
gboolean umi_git_commit (const char *dir, const char *message, GError **err);
#endif /* UMICOM_GIT_INTEGRATION_H */