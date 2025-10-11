/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/argv.c
 * PURPOSE: Command-line parsing and argument handoff for the application
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/argv.h"   /* umi_argv_apply() declaration to ensure signature match */
#include <gtk/gtk.h>
#include <glib.h>
#include <string.h>

/*-----------------------------------------------------------------------------
 * Notes
 * -----
 * - This module ONLY parses CLI switches and stores the results on the
 *   GtkApplication object via g_object_set_data_*.
 * - Actual actions (opening files, setting workspace root, etc.) are performed
 *   later in app.c (or wherever you read the stored values).
 *
 *   Stored keys (read these from app.c):
 *     "umi.arg.dir"  -> const char* directory path for the file tree
 *     "umi.arg.open" -> const char* file path to open in the editor
 *
 * - Recognized flags:
 *     --dir=<PATH>     | --dir <PATH>     | -d <PATH>
 *     --open=<FILE>    | --open <FILE>    | -o <FILE>
 *
 * - Unrecognized args are preserved (left in argv) so GTK can consume its own
 *   flags (like --gapplication-service, etc.).
 *---------------------------------------------------------------------------*/

/* Small helper to store strdup'd strings on the app with automatic free */
static inline void
umi__set_app_string(GtkApplication *app, const char *key, const char *value)
{
  if (!app || !key) return;
  if (value && *value)
    g_object_set_data_full(G_OBJECT(app), key, g_strdup(value), g_free);
}

/* Parse an option that can be either "--opt=value" or "--opt value" or "-o value"
   Returns the value pointer (owned by caller or to be duplicated by caller),
   and advances *i when it consumes a separate value token. */
static const char *
umi__take_opt_value(int ac, char **av, int *i, const char *arg,
                    const char *long_eq_prefix, const char *long_sep,
                    const char *short_sep)
{
  /* --opt=value */
  if (g_str_has_prefix(arg, long_eq_prefix)) {
    return arg + (int)strlen(long_eq_prefix);
  }

  /* --opt value */
  if (g_strcmp0(arg, long_sep) == 0) {
    if ((*i + 1) < ac) {
      (*i)++;
      return av[*i];
    }
    g_warning("Option '%s' requires a value.", long_sep);
    return NULL;
  }

  /* -o value (short) */
  if (short_sep && g_strcmp0(arg, short_sep) == 0) {
    if ((*i + 1) < ac) {
      (*i)++;
      return av[*i];
    }
    g_warning("Option '%s' requires a value.", short_sep);
    return NULL;
  }

  return NULL;
}

/*-----------------------------------------------------------------------------
 * umi_argv_apply
 *   Parse known args, stash results on the GtkApplication, and compact argv
 *   so that unrecognized args remain for GTK. Returns TRUE to continue startup.
 *---------------------------------------------------------------------------*/
gboolean
umi_argv_apply(GtkApplication *app, int *argc, char ***argv)
{
  g_return_val_if_fail(GTK_IS_APPLICATION(app), TRUE);
  g_return_val_if_fail(argc != NULL && argv != NULL && *argv != NULL, TRUE);

  int ac = *argc;
  char **av = *argv;

  /* Compact argv in-place: keep index 0 (program name), then write-through. */
  int out = 1;

  for (int i = 1; i < ac; i++) {
    const char *arg = av[i];
    if (!arg) continue;

    /* --dir=<PATH> | --dir <PATH> | -d <PATH> */
    const char *dir_val = umi__take_opt_value(
        ac, av, &i, arg, "--dir=", "--dir", "-d");
    if (dir_val) {
      umi__set_app_string(app, "umi.arg.dir", dir_val);
      continue; /* consumed; don't copy into compacted argv */
    }

    /* --open=<FILE> | --open <FILE> | -o <FILE> */
    const char *open_val = umi__take_opt_value(
        ac, av, &i, arg, "--open=", "--open", "-o");
    if (open_val) {
      umi__set_app_string(app, "umi.arg.open", open_val);
      continue; /* consumed */
    }

    /* Unknown argument â†’ keep it so GTK can parse its own flags later. */
    av[out++] = av[i];
  }

  /* Null-terminate and update argc for the caller */
  av[out] = NULL;
  *argc = out;

  return TRUE;
}
