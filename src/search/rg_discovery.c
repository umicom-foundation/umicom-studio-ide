/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/search/rg_discovery.c
 * PURPOSE: Locate the ripgrep binary and read its version banner
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#include <glib.h>
#include <string.h>
#include "rg_discovery.h"

/*-----------------------------------------------------------------------------
 * run_rg_and_capture_version:
 *   Helper that spawns 'rg --version' and captures the first line of output.
 *   On success returns a newly allocated gchar* (caller frees). On failure,
 *   returns NULL.
 *---------------------------------------------------------------------------*/
static gchar *run_rg_and_capture_version(const gchar *rg_path) {
  g_return_val_if_fail(rg_path != NULL, NULL);

  /* Build argv vector: [rg_path, "--version", NULL] */
  const gchar *argvv[] = { rg_path, "--version", NULL };

  /* Launch child process, inherit no FDs, capture out/err into pipes. */
  gchar *stdout_str = NULL;
  gchar *stderr_str = NULL;
  gint   exit_status = 0;
  GError *err = NULL;

  if (!g_spawn_sync(NULL,               /* working directory: inherit */
                    (gchar **)argvv,    /* argv */
                    NULL,               /* envp: inherit */
                    G_SPAWN_SEARCH_PATH,
                    NULL,               /* child setup */
                    NULL,               /* user data */
                    &stdout_str,        /* out */
                    &stderr_str,        /* err */
                    &exit_status,       /* status */
                    &err)) {
    if (err) g_error_free(err);
    g_free(stderr_str);
    g_free(stdout_str);
    return NULL;
  }

  /* Extract first line of stdout as the version banner. */
  gchar *nl = stdout_str ? strchr(stdout_str, '\n') : NULL;
  if (nl) *nl = '\0';

  /* Normalize empty result to NULL. */
  if (!stdout_str || *stdout_str == '\0') {
    g_free(stderr_str);
    g_free(stdout_str);
    return NULL;
  }

  g_free(stderr_str);
  /* Transfer ownership to the caller. */
  return stdout_str;
}

/*-----------------------------------------------------------------------------
 * umi_rg_discover:
 *   Probe PATH for 'rg' and capture version. Returns NULL if not found or if
 *   the spawned process fails. On success both fields of UmiRgProbe are set.
 *---------------------------------------------------------------------------*/
UmiRgProbe *umi_rg_discover(void) {
  /* Let GLib search PATH for 'rg'. */
  gchar *rg_path = g_find_program_in_path("rg");
  if (!rg_path) {
    return NULL;
  }

  /* Try to run it and obtain the banner. */
  gchar *version = run_rg_and_capture_version(rg_path);
  if (!version) {
    g_free(rg_path);
    return NULL;
  }

  /* Package into result struct. */
  UmiRgProbe *p = g_new0(UmiRgProbe, 1);
  p->path = rg_path;     /* take ownership */
  p->version = version;  /* take ownership */
  return p;
}

/*-----------------------------------------------------------------------------
 * umi_rg_probe_free:
 *   Dispose of UmiRgProbe (safe with NULL).
 *---------------------------------------------------------------------------*/
void umi_rg_probe_free(UmiRgProbe *p) {
  if (!p) return;
  g_free(p->path);
  g_free(p->version);
  g_free(p);
}
/*---------------------------------------------------------------------------*/