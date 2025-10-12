/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/panes/output/output_console.c
 *
 * PURPOSE:
 *   Implements a lightweight output console that accepts text lines and
 *   appends them into a GtkTextBuffer, while exposing a GTK widget that can
 *   be packed into the UI (a GtkScrolledWindow containing a GtkTextView).
 *
 * DESIGN CHOICES:
 *   - Opaque public type: the header only forward-declares UmiOutputConsole,
 *     so we can evolve internals without breaking consumers.
 *   - Self-contained widget: the console constructs and owns a TextView
 *     wrapped in a ScrolledWindow; callers just pack the widget.
 *   - Thread-safe appends: lines are marshalled onto the GTK main loop using
 *     g_idle_add_full() so it’s safe to call from worker threads.
 *   - Ownership policy: if constructed without a buffer, we create one and
 *     the TextView will take a reference; we won’t unref it manually to avoid
 *     double-free when the view goes away. If a caller passes a buffer, we
 *     do not own it and we never unref it.
 *   - No cross-module includes: avoids tangled dependencies; strictly uses
 *     its own public header and GTK/GLib.
 *
 * RISK & SAFETY NOTES:
 *   - No raw pointer exposure outside the module; dangling pointer risk is
 *     minimized. All UI mutations run on the GTK main thread.
 *   - Buffer overflows are avoided by using GLib/GTK APIs that manage sizes.
 *   - Stack overflows are not expected; we avoid deep recursion entirely.
 *
 * REQUIREMENTS / LIBS:
 *   - GTK 4 (GtkTextView, GtkScrolledWindow)
 *   - GLib / GObject (idle, GString, g_autofree helpers)
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>                     /* GTK widgets, text buffer, idle API  */
#include <glib.h>                        /* GLib base types/utilities            */
#include "output_console.h"              /* Public console API (opaque type)     */
/* NOTE:
 * We intentionally do NOT include any cross-module headers such as
 * "include/diagnostic_parsers.h". If we later add optional parsing,
 * we’ll inject it via a tiny hook interface to keep modules decoupled.
 */

/*-----------------------------------------------------------------------------
 * INTERNAL TYPES
 *---------------------------------------------------------------------------*/

/* Forward declaration for a possible ANSI helper (kept extensible).
 * We stub it out for now; future work can plug in a real parser without
 * changing this module’s external interface.
 */
typedef struct _UmiAnsi UmiAnsi;

/* A tiny sink/chain to allow chaining of line processors (extensible).          */
typedef struct _UmiOutChain {
  void (*sink)(void *user, const char *line, gboolean is_err); /* next sink fns  */
  void  *user;                                                 /* user context   */
} UmiOutChain;

/* Opaque console instance – internal definition.                                */
struct _UmiOutputConsole {
  GtkTextBuffer *buf;                 /* target buffer that receives lines       */
  UmiAnsi       *ansi;                /* optional ANSI color parser (may be NULL)*/
  UmiOutChain    chain;               /* downstream sink chain                   */
  gboolean       created_buf;         /* TRUE if we created the buffer           */

  /* The public getter returns this scrolled window for packing into UI.          */
  GtkWidget     *widget;              /* GtkScrolledWindow handed to caller      */
  GtkWidget     *view;                /* GtkTextView displaying `buf`            */
};

/* Small payload for idle append (main-thread marshalling).                      */
typedef struct {
  UmiOutputConsole *self;             /* console instance to mutate              */
  char             *line;             /* heap-dup’d text to insert               */
} IdleAppend;

/*-----------------------------------------------------------------------------
 * INTERNAL HELPERS
 *---------------------------------------------------------------------------*/

/* Optional ANSI parser construction/destruction (stubs if not used).           */
static UmiAnsi *umi_ansi_new(void) { return NULL; }           /* no-op stub      */
static void     umi_ansi_free(UmiAnsi *a) { (void)a; }        /* no-op stub      }

/* Append a single line to the GtkTextBuffer on the GTK main loop.
 * This function runs from the idle handler (main thread).
 */
static gboolean
idle_append_cb(gpointer data)
{
  IdleAppend *p = (IdleAppend *)data;                       /* payload          */
  if (!p || !p->self || !p->line) {                         /* guard            */
    if (p) g_free(p);                                       /* cleanup payload  */
    return G_SOURCE_REMOVE;                                  /* one-shot done    */
  }

  GtkTextBuffer *buf = p->self->buf;                        /* the buffer       */

  gtk_text_buffer_insert_at_cursor(buf, p->line, -1);       /* insert text      */
  gtk_text_buffer_insert_at_cursor(buf, "\n", 1);           /* newline          */

  GtkTextIter end_iter;                                     /* auto-scroll      */
  gtk_text_buffer_get_end_iter(buf, &end_iter);             /* compute end      */
  gtk_text_buffer_place_cursor(buf, &end_iter);             /* place cursor     */

  g_free(p->line);                                          /* free text        */
  g_free(p);                                                /* free payload     */
  return G_SOURCE_REMOVE;                                   /* remove idle      */
}

/* Deliver a line into the console pipeline (console -> buffer via idle).         */
static void
console_sink(void *user, const char *line, gboolean is_err)
{
  (void)is_err;                                             /* styling hook     */
  UmiOutputConsole *c = (UmiOutputConsole *)user;           /* console          */
  if (!c || !line) return;                                  /* guard            */

  IdleAppend *payload = g_new(IdleAppend, 1);               /* allocate         */
  payload->self = c;                                        /* set console      */
  payload->line = g_strdup(line);                           /* duplicate text   */

  g_idle_add_full(G_PRIORITY_DEFAULT, idle_append_cb, payload, NULL); /* enqueue */
}

/*-----------------------------------------------------------------------------
 * PUBLIC API IMPLEMENTATION
 *---------------------------------------------------------------------------*/

/* Create a new console (optionally with an existing GtkTextBuffer).              */
UmiOutputConsole *
umi_output_console_new(GtkTextBuffer *opt_buf)
{
  UmiOutputConsole *c = g_new0(UmiOutputConsole, 1);        /* zero-init        */

  if (opt_buf) {                                            /* caller-supplied  */
    c->buf = opt_buf;                                       /* non-owning       */
    c->created_buf = FALSE;                                  /* reflect          */
  } else {                                                  /* create our own   */
    c->buf = gtk_text_buffer_new(NULL);                     /* default buffer   */
    c->created_buf = TRUE;                                   /* mark created     */
    /* We do NOT manually unref this later; the TextView holds a ref and
     * will release it when the widget hierarchy is destroyed.                   */
  }

  c->ansi = umi_ansi_new();                                 /* optional helper  */
  c->chain.sink = console_sink;                             /* default sink     */
  c->chain.user = c;                                        /* back-reference   */

  /* Build the widget tree: TextView inside ScrolledWindow.                      */
  GtkWidget *tv = gtk_text_view_new_with_buffer(c->buf);    /* bind to buffer   */
  gtk_text_view_set_monospace(GTK_TEXT_VIEW(tv), TRUE);     /* console font     */
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tv), GTK_WRAP_CHAR); /* wrap        */
  gtk_text_view_set_editable(GTK_TEXT_VIEW(tv), FALSE);     /* read-only        */
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(tv), FALSE); /* hide caret     */

  GtkWidget *scr = gtk_scrolled_window_new();               /* GTK4 constructor */
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scr),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_AUTOMATIC);     /* scrollbars       */
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scr), tv); /* add child     */

  c->view   = tv;                                           /* store view       */
  c->widget = scr;                                          /* store widget     */
  return c;                                                 /* return handle    */
}

/* Return the downstream sink chain (so other modules can feed lines in).         */
UmiOutChain *
umi_output_console_chain(UmiOutputConsole *c)
{
  if (!c) return NULL;                                      /* guard            */
  return &c->chain;                                         /* embedded member  */
}

/* Return the GtkTextBuffer used by this console (non-owning).                    */
GtkTextBuffer *
umi_output_console_buffer(UmiOutputConsole *c)
{
  if (!c) return NULL;                                      /* guard            */
  return c->buf;                                            /* the buffer       */
}

/* Destroy the console and its internal helpers.                                  */
void
umi_output_console_free(UmiOutputConsole *c)
{
  if (!c) return;                                           /* guard            */

  if (c->ansi) {                                            /* drop helper      */
    umi_ansi_free(c->ansi);                                 /* free helper      */
    c->ansi = NULL;                                         /* clear pointer    */
  }

  if (c->widget) {                                          /* unref widget     */
    g_object_unref(c->widget);                              /* release our ref  */
    c->widget = NULL;                                       /* clear pointer    */
  }
  c->view = NULL;                                           /* child owned by scroller */

  /* IMPORTANT: Do not unref the buffer explicitly. The TextView owns a ref and
   * will release it when the widget hierarchy goes away. If the buffer was
   * supplied by the caller, we definitely must not unref it here.               */

  g_free(c);                                                /* free instance    */
}

/* Append a line (thread-safe).                                                    */
void
umi_output_console_append_line(UmiOutputConsole *c, const char *line)
{
  if (!c || !line) return;                                  /* guard            */
  IdleAppend *payload = g_new(IdleAppend, 1);               /* allocate         */
  payload->self = c;                                        /* set console      */
  payload->line = g_strdup(line);                           /* duplicate text   */
  g_idle_add_full(G_PRIORITY_DEFAULT, idle_append_cb, payload, NULL); /* enqueue */
}

/* Expose the console widget (GtkScrolledWindow).                                  */
GtkWidget *
umi_output_console_widget(UmiOutputConsole *c)
{
  if (!c) return NULL;                                      /* guard            */
  return c->widget;                                         /* scroller to pack */
}
