/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build_runner.c
 * PURPOSE: GSubprocess-based runner streaming lines to Output pane
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/build_runner.h"

typedef struct _UmiBuildRunner {
  UmiOutputPane *out;
  GSubprocess *proc;
  GDataInputStream *out_r;
  GDataInputStream *err_r;
  GCancellable *cancel;
  UmiBuildExitCb on_exit;
  gpointer user;
} UmiBuildRunner;

/* Forward decls */
static void read_next_stdout(UmiBuildRunner *br);
static void read_next_stderr(UmiBuildRunner *br);

static void append_line(UmiBuildRunner *br, const char *s, gboolean is_err){
  if(!br || !s) return;
  if(!br->out) return;
  if(is_err) umi_output_pane_append_line_err(br->out, s);
  else       umi_output_pane_append_line(br->out, s);
}

UmiBuildRunner *umi_build_runner_new(UmiOutputPane *out){
  UmiBuildRunner *br = g_new0(UmiBuildRunner,1);
  br->out = out;
  br->cancel = g_cancellable_new();
  return br;
}

static void on_read_stdout(GObject *src, GAsyncResult *res, gpointer u){
  UmiBuildRunner *br = (UmiBuildRunner*)u;
  gsize len=0; GError *e=NULL;
  gchar *line = g_data_input_stream_read_line_finish(G_DATA_INPUT_STREAM(src), res, &len, &e);
  if(e){ g_error_free(e); line=NULL; }
  if(line){
    append_line(br, line, FALSE);
    g_free(line);
    read_next_stdout(br);
  }
}

static void on_read_stderr(GObject *src, GAsyncResult *res, gpointer u){
  UmiBuildRunner *br = (UmiBuildRunner*)u;
  gsize len=0; GError *e=NULL;
  gchar *line = g_data_input_stream_read_line_finish(G_DATA_INPUT_STREAM(src), res, &len, &e);
  if(e){ g_error_free(e); line=NULL; }
  if(line){
    append_line(br, line, TRUE);
    g_free(line);
    read_next_stderr(br);
  }
}

static void read_next_stdout(UmiBuildRunner *br){
  if(!br || !br->out_r) return;
  g_data_input_stream_read_line_async(br->out_r, G_PRIORITY_DEFAULT, br->cancel, on_read_stdout, br);
}
static void read_next_stderr(UmiBuildRunner *br){
  if(!br || !br->err_r) return;
  g_data_input_stream_read_line_async(br->err_r, G_PRIORITY_DEFAULT, br->cancel, on_read_stderr, br);
}

static void on_proc_exited(GObject *src, GAsyncResult *res, gpointer u){
  UmiBuildRunner *br = (UmiBuildRunner*)u;
  int status = 0; GError *e=NULL;
  if(!g_subprocess_wait_finish(G_SUBPROCESS(src), res, &e)){
    if(e){ status = -1; g_error_free(e); }
  }else{
    status = g_subprocess_get_exit_status(G_SUBPROCESS(src));
  }
  if(br->on_exit) br->on_exit(br->user, status);
  append_line(br, "[runner] process completed", FALSE);
  g_clear_object(&br->proc);
  g_clear_object(&br->out_r); g_clear_object(&br->err_r);
}

gboolean umi_build_runner_run(UmiBuildRunner *br,
                              const char *cwd,
                              char * const *argv,
                              UmiBuildExitCb on_exit,
                              gpointer user,
                              GError **err){
  if(!br || !argv || !*argv) return FALSE;
  if(br->proc) { g_set_error(err, g_quark_from_string("umicom"), 1, "process already running"); return FALSE; }

  br->on_exit = on_exit; br->user = user;

  GSubprocessFlags flags = (GSubprocessFlags)(G_SUBPROCESS_FLAGS_STDOUT_PIPE | G_SUBPROCESS_FLAGS_STDERR_PIPE);
  GError *e=NULL;
  br->proc = g_subprocess_newv((const gchar * const*)argv, flags, &e);
  if(!br->proc){
    if(err && e) *err = e; else if(e) g_error_free(e);
    return FALSE;
  }
  if(cwd && *cwd){
    /* GLib's GSubprocess doesn't change cwd directly; users should switch cwd or pass in shell */
    append_line(br, "[runner] note: cwd parameter is advisory; ensure process runs in desired directory", FALSE);
  }

  GInputStream *so = g_subprocess_get_stdout_pipe(br->proc);
  GInputStream *se = g_subprocess_get_stderr_pipe(br->proc);
  br->out_r = g_data_input_stream_new(so);
  br->err_r = g_data_input_stream_new(se);
  g_data_input_stream_set_newline_type(br->out_r, G_DATA_STREAM_NEWLINE_TYPE_ANY);
  g_data_input_stream_set_newline_type(br->err_r, G_DATA_STREAM_NEWLINE_TYPE_ANY);

  append_line(br, "[runner] started", FALSE);

  read_next_stdout(br);
  read_next_stderr(br);

  g_subprocess_wait_async(br->proc, br->cancel, on_proc_exited, br);
  return TRUE;
}

void umi_build_runner_stop(UmiBuildRunner *br){
  if(!br || !br->proc) return;
  g_cancellable_cancel(br->cancel);
  g_subprocess_force_exit(br->proc);
  append_line(br, "[runner] stop requested", TRUE);
}

void umi_build_runner_free(UmiBuildRunner *br){
  if(!br) return;
  if(br->proc) umi_build_runner_stop(br);
  g_clear_object(&br->out_r); g_clear_object(&br->err_r);
  g_clear_object(&br->proc);
  g_clear_object(&br->cancel);
  g_free(br);
}
