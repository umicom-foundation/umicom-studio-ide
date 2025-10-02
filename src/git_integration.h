/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/git_integration.h
 * PURPOSE: Minimal Git helpers (status/add/commit) via subprocess
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_GIT_INTEGRATION_H
#define UMICOM_GIT_INTEGRATION_H

#include <gio/gio.h>

/* These helpers run the 'git' command in the given working directory and return
 * the textual output. They are intentionally small and easy to read for newcomers. */
gchar *umi_git_status(const char *cwd, GError **err);
gchar *umi_git_add_all(const char *cwd, GError **err);
gchar *umi_git_commit (const char *cwd, const char *message, GError **err);

#endif /* UMICOM_GIT_INTEGRATION_H */
