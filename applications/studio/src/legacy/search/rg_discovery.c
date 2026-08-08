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

  const gchar *argvv[] = { rg_path, "--version", NULL };

  gchar *stdout_str = NULL;
  gchar *stderr_str = NULL;
  gint   exit_status = 0;
  GError *err = NULL;

  if (!g_spawn_sync(NULL,
                    (gchar **)argvv,
                    NULL,
                    G_SPAWN_SEARCH_PATH,
                    NULL,
                    NULL,
                    &stdout_str,
                    &stderr_str,
                    &exit_status,
                    &err)) {
    if (err) g_error_free(err);
    g_free(stderr_str);
    g_free(stdout_str);
    return NULL;
  }

  gchar *nl = stdout_str ? strchr(stdout_str, '\n') : NULL;
  if (nl) *nl = '\0';

  if (!stdout_str || *stdout_str == '\0') {
    g_free(stderr_str);
    g_free(stdout_str);
    return NULL;
  }

  g_free(stderr_str);
  return stdout_str; /* caller owns */
}

/*-----------------------------------------------------------------------------
 * umi_rg_discover:
 *   Probe PATH for 'rg' and capture version. Returns NULL if not found or if
 *   the spawned process fails. On success both fields of UmiRgProbe are set.
 *---------------------------------------------------------------------------*/
UmiRgProbe *umi_rg_discover(void) {
  gchar *rg_path = g_find_program_in_path("rg");
  if (!rg_path) {
    return NULL;
  }

  gchar *version = run_rg_and_capture_version(rg_path);
  if (!version) {
    g_free(rg_path);
    return NULL;
  }

  UmiRgProbe *p = g_new0(UmiRgProbe, 1);
  p->path = rg_path;
  p->version = version;
  return p;
}

void umi_rg_probe_free(UmiRgProbe *p) {
  if (!p) return;
  g_free(p->path);
  g_free(p->version);
  g_free(p);
}
/*---------------------------------------------------------------------------*/
