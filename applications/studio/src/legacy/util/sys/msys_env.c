/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/msys_env.c
 *
 * PURPOSE:
 *   Discover MSYS/MinGW roots on Windows and build a PATH prefix hint for
 *   spawning tools that require those runtimes.
 *
 * API:
 *   gboolean umi_msys_detected(void);
 *   gchar   *umi_msys_path_hint(void);   // g_free()
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#include "msys_env.h"
#include <glib.h>
#include <string.h>
#include <stdlib.h>

#if defined(G_OS_WIN32)
#include <windows.h>
#endif

static gboolean path_is_dir(const char *path) {
  return (path && *path) ? g_file_test(path, G_FILE_TEST_IS_DIR) : FALSE;
}

static const char * const g_msys_prefixes[] = {
  "C:\\\\msys64",
  "C:\\\\tools\\\\msys64",
  NULL
};

gboolean umi_msys_detected(void) {
#if defined(G_OS_WIN32)
  for (const char * const *p = g_msys_prefixes; *p; ++p)
    if (path_is_dir(*p)) return TRUE;
  const char *env = g_getenv("MSYS2_BASE");
  return env && path_is_dir(env);
#else
  return FALSE;
#endif
}

gchar *umi_msys_path_hint(void) {
#if !defined(G_OS_WIN32)
  return NULL;
#else
  const char *base = NULL;

  const char *env = g_getenv("MSYS2_BASE");
  if (env && path_is_dir(env)) base = env;
  if (!base) {
    for (const char * const *p = g_msys_prefixes; *p; ++p)
      if (path_is_dir(*p)) { base = *p; break; }
  }
  if (!base) return NULL;

  gchar *usr_bin   = g_build_filename(base, "usr",    "bin", NULL);
  gchar *mingw_bin = g_build_filename(base, "mingw64","bin", NULL);

  GString *out = g_string_new(NULL);
  if (path_is_dir(usr_bin))   g_string_append(out, usr_bin);
  if (path_is_dir(mingw_bin)) {
    if (out->len) g_string_append_c(out, ';');
    g_string_append(out, mingw_bin);
  }

  g_free(usr_bin);
  g_free(mingw_bin);

  if (out->len == 0) { g_string_free(out, TRUE); return NULL; }
  return g_string_free(out, FALSE);
#endif
}
