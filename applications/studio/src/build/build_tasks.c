/*-----------------------------------------------------------------------------
 * Umicom Studio IDE : OpenSource IDE for developers and Content Creators
 * Repository: https://github.com/umicom-foundation/umicom-studio-ide
 * File: src/build/build_tasks.c
 *
 * PURPOSE:
 *   Orchestrates build/run/test for a given project root.
 *
 * DESIGN:
 *   No direct UI types. Streams diagnostics through UmiOutputSink; uses
 *   UmiDiagParser to normalize output.
 *
 * API:
 *   umi_build_tasks_new/free/build/run/test/root
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <stdarg.h>             /* va_list for emit() helper               */
#include <glib.h>
#include <gio/gio.h>
#include "build_tasks.h"
#include "diagnostic_parsers.h"
#include "umi_output_sink.h"

struct _UmiBuildTasks {
  gchar          *root;         /* project root directory (UTF-8)         */
  UmiOutputSink  *sink;         /* where we print user-visible messages   */
};

/* Emit a simple message to the sink (defensive if sink is NULL). */
static void emit(UmiBuildTasks *t, UmiDiagSeverity sev, const char *fmt, ...) {
  if (!t || !t->sink || !fmt) return;
  va_list ap; va_start(ap, fmt);
  gchar *s = g_strdup_vprintf(fmt, ap);
  va_end(ap);

  UmiDiag d = {0};
  d.severity = sev; d.file = g_strdup(""); d.message = g_strdup(s); d.line = 0; d.column = 0;
  umi_output_sink_emit(t->sink, &d);
  g_free(d.file); g_free(d.message); g_free(s);
}

UmiBuildTasks *umi_build_tasks_new(const char *root, UmiOutputSink *sink) {
  UmiBuildTasks *t = g_new0(UmiBuildTasks, 1);
  t->root = g_strdup(root ? root : ".");
  t->sink = sink;
  emit(t, UMI_DIAG_NOTE, "BuildTasks: initialized for root='%s'", t->root);
  return t;
}

void umi_build_tasks_free(UmiBuildTasks *t) {
  if (!t) return;
  g_clear_pointer(&t->root, g_free);
  g_free(t);
}

void umi_build_tasks_set_sink(UmiBuildTasks *t, UmiOutputSink *sink) {
  if (!t) { return; }          /* (fixed misleading indentation warning) */
  t->sink = sink;
}

/* Accessor declared in the header. */
const char *umi_build_tasks_root(const UmiBuildTasks *t) {
  return t ? t->root : NULL;
}

/* Minimal probe: runs "ninja --version" and streams output through parser.
 * Replace later with real build/run invocations (tool-specific argv).
 */
static gboolean run_tool_and_parse(UmiBuildTasks *t,
                                   const char   *cmd,
                                   char * const  argv[],
                                   GError      **error)
{
  if (!t || !cmd) { g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT, "invalid args"); return FALSE; }

  /* Use varargs constructor; tolerate argv[1] being NULL (no extra arg). */
  GSubprocessFlags flags = G_SUBPROCESS_FLAGS_STDOUT_PIPE | G_SUBPROCESS_FLAGS_STDERR_PIPE;
  GSubprocess *sp = (argv && argv[1])
                    ? g_subprocess_new(flags, error, cmd, argv[1], NULL)
                    : g_subprocess_new(flags, error, cmd, NULL);
  if (!sp) return FALSE;

  GInputStream *out = g_subprocess_get_stdout_pipe(sp);
  GDataInputStream *din = g_data_input_stream_new(out);

  UmiDiagParser *parser = umi_diag_parser_new("ninja");
  while (TRUE) {
    gsize len = 0; GError *e = NULL;
    gchar *line = g_data_input_stream_read_line(din, &len, NULL, &e);
    if (!line || e) { g_clear_error(&e); g_free(line); break; }

    UmiDiag *diag = NULL;
    if (umi_diag_parser_feed_line(parser, line, &diag)) {
      umi_output_sink_emit(t->sink, diag);
      umi_diag_free(diag);
    } else {
      emit(t, UMI_DIAG_NOTE, "%s", line);
    }
    g_free(line);
  }

  g_object_unref(din);
  g_subprocess_wait(sp, NULL, NULL);
  g_object_unref(sp);
  umi_diag_parser_free(parser);
  return TRUE;
}

gboolean umi_build_tasks_build(UmiBuildTasks *t, GError **error) {
  if (!t) return FALSE;
  char * const argv[] = { "ninja", "--version", NULL };
  return run_tool_and_parse(t, "ninja", argv, error);
}

gboolean umi_build_tasks_run(UmiBuildTasks *t, GError **error) {
  (void)error;
  emit(t, UMI_DIAG_NOTE, "Run not implemented yet");
  return TRUE;
}

gboolean umi_build_tasks_test(UmiBuildTasks *t, GError **error) {
  (void)error;
  emit(t, UMI_DIAG_NOTE, "Test not implemented yet");
  return TRUE;
}

/*  END OF FILE */
