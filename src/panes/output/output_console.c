/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/panes/output/output_console.c
 *
 * PURPOSE:
 *   Minimal, thread-safe text console that renders process output into a
 *   GtkTextBuffer. Exposes a small “sink chain” adapter (UmiOutChain) so other
 *   subsystems can write lines without depending on GTK details.
 *
 * DESIGN CHOICES:
 *   - Loosely coupled: only depends on sibling public headers:
 *       - include/output_console.h  (this module’s API)
 *       - include/output_filters.h  (ANSI color/attribute filter)
 *   - Thread-safe appends: any thread may call append; we marshal to the GTK
 *     main loop via g_idle_add_full().
 *   - No ownership surprises: we never unref the GtkTextBuffer; the view (or
 *     caller) owns it. If NULL is passed to _new(), we create a fresh buffer
 *     and still do not unref it here (caller/view should take ownership).
 *   - Warnings-clean C11 with GLib/GTK only; no cross-folder includes.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <glib.h>
#include <string.h>

/* Local public headers (sibling include/ folder) */
#include "include/output_console.h"   /* UmiOutputConsole, UmiOutChain API */
#include "include/output_filters.h"   /* UmiAnsi, umi_ansi_new, umi_ansi_append_line */

/*========================
 * Internal data structure
 *========================*/
struct _UmiOutputConsole {
  GtkTextBuffer *buf;   /* Target buffer for rendered output (not owned). */
  UmiAnsi       *ansi;  /* ANSI filter that writes into buf (owned).      */
  UmiOutChain    chain; /* Adapter so pipelines can call ->sink(line,...).*/
  gboolean       created_buf; /* TRUE if we created a fresh buffer.       */
};

/*=========================================
 * Main-loop marshaling for line append
 *=========================================*/

/* Small payload carried to GTK main loop */
typedef struct {
  UmiOutputConsole *self;
  char             *line;  /* heap copy of the line */
} IdleAppend;

/* Runs on GTK main loop; safe to touch GtkTextBuffer here. */
static gboolean
idle_append_cb(gpointer data)
{
  IdleAppend *p = (IdleAppend*)data;
  if (G_UNLIKELY(!p || !p->self || !p->self->ansi || !p->line)) {
    g_free(p ? p->line : NULL);
    g_free(p);
    return G_SOURCE_REMOVE;
  }

  /* Let the ANSI filter parse/insert styled text into the buffer. */
  umi_ansi_append_line(p->self->ansi, p->line);

  g_free(p->line);
  g_free(p);
  return G_SOURCE_REMOVE; /* one-shot */
}

/*=========================================
 * UmiOutChain sink implementation
 *=========================================*/
static void
chain_sink(const char *line, gpointer user)
{
  UmiOutputConsole *self = (UmiOutputConsole*)user;
  if (!self || !line) return;

  /* Any thread can call this; hand off to GTK thread. */
  IdleAppend *payload = g_new(IdleAppend, 1);
  payload->self = self;
  payload->line = g_strdup(line);
  /* Use default priority; free payload in callback. */
  g_idle_add_full(G_PRIORITY_DEFAULT, idle_append_cb, payload, NULL);
}

/*========================
 * Public API
 *========================*/

UmiOutputConsole *
umi_output_console_new(GtkTextBuffer *buf)
{
  UmiOutputConsole *c = g_new0(UmiOutputConsole, 1);

  /* Buffer policy:
     - If caller supplies one, we use it (do NOT ref/unref here).
     - If NULL, create a fresh buffer; still do NOT unref on free(), since
       ownership normally lives with the GtkTextView that will attach to it.
  */
  if (buf) {
    c->buf = buf;
    c->created_buf = FALSE;
  } else {
    c->buf = gtk_text_buffer_new(NULL);
    c->created_buf = TRUE; /* informational; no unref in _free() by design */
  }

  /* Create ANSI filter that writes styled text into our buffer. */
  c->ansi = umi_ansi_new(c->buf);

  /* Prepare the chain adapter so other modules can push lines. */
  c->chain.sink      = chain_sink;
  c->chain.user      = c;
  c->chain.free_user = NULL; /* not needed; console owns itself */

  return c;
}

void
umi_output_console_free(UmiOutputConsole *c)
{
  if (!c) return;

  /* Destroy ANSI helper. */
  if (c->ansi) {
    umi_ansi_free(c->ansi);
    c->ansi = NULL;
  }

  /* DO NOT unref c->buf (ownership is external by contract).
     Even if we created it, a view likely took ownership. */

  g_free(c);
}

void
umi_output_console_append_line(UmiOutputConsole *c, const char *line)
{
  if (!c || !line) return;

  /* Marshal to GTK main loop—safe from any thread. */
  IdleAppend *payload = g_new(IdleAppend, 1);
  payload->self = c;
  payload->line = g_strdup(line);
  g_idle_add_full(G_PRIORITY_DEFAULT, idle_append_cb, payload, NULL);
}

GtkTextBuffer *
umi_output_console_buffer(UmiOutputConsole *c)
{
  return c ? c->buf : NULL;
}

UmiOutChain *
umi_output_console_chain(UmiOutputConsole *c)
{
  return c ? &c->chain : NULL;
}
/*--------------------------------- End of file --------------------------------*/