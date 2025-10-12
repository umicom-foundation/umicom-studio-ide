/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/msys_env.c
 * PURPOSE:
 *   Discover MSYS/MinGW roots on Windows and build a PATH prefix hint that
 *   callers can prepend when spawning subprocesses that need MSYS tools.
 *
 * DESIGN:
 *   - Windows-only behavior, but compiles on other platforms (returns NULL).
 *   - Uses GLib file utilities and environment helpers.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-01
 * License: MIT
 *---------------------------------------------------------------------------*/
#include "msys_env.h"   /* declarations */      /* NOLINT */
#include <string.h>
#include <stdlib.h>
#include <glib.h>

#if defined(G_OS_WIN32)
#include <windows.h>
#endif

/* Internal helper: return TRUE if @path exists as a directory. */
static gboolean path_is_dir(const char *path) {
  if (!path || !*path) return FALSE;
  return g_file_test(path, G_FILE_TEST_IS_DIR);
}

/* Try common MSYS2 prefixes; callers may expand this later via settings. */
static const char * const g_msys_prefixes[] = {
  "C:\\\\msys64",
  "C:\\\\tools\\\\msys64",
  NULL
};

gboolean umi_msys_detected(void) {
#if defined(G_OS_WIN32)
  for (const char * const *p = g_msys_prefixes; *p; ++p) {
    if (path_is_dir(*p)) return TRUE;
  }
  /* Also consider MSYS2_BASE env var if user customized location. */
  const char *env = g_getenv("MSYS2_BASE");
  return env && path_is_dir(env);
#else
  return FALSE;
#endif
}

/* Build "C:\msys64\usr\bin;C:\msys64\mingw64\bin" if present; else NULL. */
gchar *umi_msys_path_hint(void) {
#if !defined(G_OS_WIN32)
  return NULL;
#else
  const char *base = NULL;

  /* Prefer env override. */
  const char *env = g_getenv("MSYS2_BASE");
  if (env && path_is_dir(env)) base = env;

  /* Fall back to known prefixes. */
  if (!base) {
    for (const char * const *p = g_msys_prefixes; *p; ++p) {
      if (path_is_dir(*p)) { base = *p; break; }
    }
  }
  if (!base) return NULL;

  /* Compose candidate bin directories. */
  gchar *usr_bin   = g_build_filename(base, "usr",   "bin", NULL);
  gchar *mingw_bin = g_build_filename(base, "mingw64", "bin", NULL);

  GString *out = g_string_new(NULL);
  if (path_is_dir(usr_bin))   g_string_append(out, usr_bin);
  if (path_is_dir(mingw_bin)) {
    if (out->len) g_string_append_c(out, ';');
    g_string_append(out, mingw_bin);
  }

  g_free(usr_bin);
  g_free(mingw_bin);

  if (out->len == 0) { g_string_free(out, TRUE); return NULL; }
  return g_string_free(out, FALSE); /* transfer */
#endif
}
