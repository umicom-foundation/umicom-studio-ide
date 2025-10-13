/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/build_tasks.c
 *
 * PURPOSE:
 *   Loosely-coupled implementation of the "Build Tasks" façade declared in
 *   src/build/include/build_tasks.h. All text is sent through UmiOutputSink.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/
#include <glib.h>
#include "build_tasks.h"

struct _UmiBuildTasks {
  gchar          *root;        /* Project root directory                     */
  UmiBuildSys    *sys;         /* Build system descriptor                     */
  UmiBuildRunner *runner;      /* Process runner                              */
  UmiOutputSink   sink;        /* Output line callback (may be NULL)          */
  gpointer        sink_user;   /* Opaque user-data passed to sink             */
};

static inline void append_gerror(GString *err, const GError *gerr)
{
  if (!err) return;
  if (gerr && gerr->message) g_string_append_printf(err, "%s\n", gerr->message);
  else                       g_string_append(err, "unknown error\n");
}

UmiBuildTasks *umi_build_tasks_new(const char *root, UmiOutputSink sink, gpointer sink_user)
{
  UmiBuildTasks *bt = g_new0(UmiBuildTasks, 1);
  bt->root      = g_strdup(root ? root : ".");
  bt->sys       = umi_buildsys_detect(bt->root);
  bt->runner    = umi_build_runner_new();
  bt->sink      = sink;
  bt->sink_user = sink_user;
  /* Ensure runner forwards output to our sink. */
  umi_build_runner_set_sink(bt->runner, bt->sink, bt->sink_user);
  return bt;
}

void umi_build_tasks_free(UmiBuildTasks *bt)
{
  if (!bt) return;
  if (bt->runner) { umi_build_runner_free(bt->runner); bt->runner = NULL; }
  if (bt->sys)    { umi_buildsys_free(bt->sys);         bt->sys    = NULL; }
  g_clear_pointer(&bt->root, g_free);
  g_free(bt);
}

void umi_build_tasks_set_sink(UmiBuildTasks *bt, UmiOutputSink sink, gpointer sink_user)
{
  if (!bt) return;
  bt->sink      = sink;
  bt->sink_user = sink_user;
  umi_build_runner_set_sink(bt->runner, bt->sink, bt->sink_user);
}

/* Small helper to run a prepared argv vector via the runner. */
static gboolean run_argv(UmiBuildTasks *bt, GPtrArray *argv, GString *err)
{
  if (!bt || !bt->runner || !argv) return FALSE;
  char * const *argvv = (char * const *)argv->pdata;
  GError *gerr = NULL;
  gboolean ok = umi_build_runner_run(bt->runner, argvv, NULL, bt->root,
                                     /* on_exit */ NULL, /* user */ bt,
                                     &gerr);
  if (!ok) append_gerror(err, gerr), g_clear_error(&gerr);
  return ok;
}

gboolean umi_build_tasks_build(UmiBuildTasks *bt, GString *err)
{
  if (!bt || !bt->sys) return FALSE;
  GPtrArray *argv = umi_buildsys_build_argv(bt->sys);
  gboolean ok = run_argv(bt, argv, err);
  if (argv) g_ptr_array_free(argv, TRUE);
  return ok;
}

gboolean umi_build_tasks_run(UmiBuildTasks *bt, GString *err)
{
  if (!bt || !bt->sys) return FALSE;
  GPtrArray *argv = umi_buildsys_run_argv(bt->sys);
  gboolean ok = run_argv(bt, argv, err);
  if (argv) g_ptr_array_free(argv, TRUE);
  return ok;
}

gboolean umi_build_tasks_test(UmiBuildTasks *bt, GString *err)
{
  if (!bt || !bt->sys) return FALSE;
  GPtrArray *argv = umi_buildsys_test_argv(bt->sys);
  gboolean ok = run_argv(bt, argv, err);
  if (argv) g_ptr_array_free(argv, TRUE);
  return ok;
}

const char *umi_build_tasks_root(const UmiBuildTasks *bt)
{
  return bt ? bt->root : NULL;
}
