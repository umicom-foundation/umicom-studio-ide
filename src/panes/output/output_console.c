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
 *   - Self-contained widget: the console now constructs and owns a TextView
 *     wrapped in a ScrolledWindow; callers just pack the widget.
 *   - Thread-safe appends: lines are marshalled onto the GTK main loop using
 *     g_idle_add_full() so it’s safe to call from worker threads.
 *   - Ownership policy: if constructed without a buffer, we create one and
 *     the TextView will take a reference; we won’t unref it manually to avoid
 *     double-free when the view goes away. If a caller passes a buffer, we
 *     do not own it and we never unref it.
 *   - Extensible output chain: a tiny sink/chain API lets other parts hook or
 *     transform lines before they hit the buffer (e.g., ANSI color parsing).
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
#include "include/output_console.h"      /* Public console API (opaque type)     */
#include "include/diagnostic_parsers.h"  /* Optional: parse build lines to spans */

/*-----------------------------------------------------------------------------
 * INTERNAL TYPES
 *---------------------------------------------------------------------------*/

/* Forward declaration for ANSI helper (kept as-is from existing code).          */
typedef struct _UmiAnsi UmiAnsi;

/* A small link/sink to allow chaining of line processors.                       */
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

  /* NEW: widget fields (to fix compile error and make the console self-contained)
   * We keep the scrolled window as the “widget” we expose; it owns the child view.
   */
  GtkWidget     *widget;              /* GtkScrolledWindow we hand to the caller */
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
static UmiAnsi *
umi_ansi_new(void)
{
  /* For now return NULL to indicate “no ANSI processing”.                        */
  return NULL;                        /* No heap allocation => no leak risk      */
}

static void
umi_ansi_free(UmiAnsi *a)
{
  /* Nothing to free yet (kept for symmetry / future expansion).                  */
  (void)a;
}

/* Append a single line to the GtkTextBuffer on the GTK main loop.
 * This function runs from the idle handler (main thread).
 */
static gboolean
idle_append_cb(gpointer data)
{
  IdleAppend *p = (IdleAppend *)data; /* payload with console+line               */

  /* Guard: ensure payload and console exist.                                     */
  if (!p || !p->self || !p->line) {
    if (p) g_free(p);                 /* free payload if partially valid         */
    return G_SOURCE_REMOVE;           /* remove source; nothing to do            */
  }

  /* Obtain buffer from console.                                                  */
  GtkTextBuffer *buf = p->self->buf;  /* buffer is owned by view; not NULL       */

  /* Insert the line at the end (cursor position).                                */
  gtk_text_buffer_insert_at_cursor(buf, p->line, -1);  /* -1 => NUL-terminated  */
  gtk_text_buffer_insert_at_cursor(buf, "\n", 1);      /* newline after each    */

  /* Auto-scroll to the end: move iter to end and place cursor there.             */
  GtkTextIter end_iter;               /* iterator to represent end-of-buffer     */
  gtk_text_buffer_get_end_iter(buf, &end_iter);        /* compute end           */
  gtk_text_buffer_place_cursor(buf, &end_iter);        /* move cursor           */

  /* If a view exists, ensure it scrolls (via “insert-text” default behavior).    */
  /* NOTE: In GTK4, the default behavior keeps the insert mark visible.           */

  /* Clean up heap allocations from the payload.                                  */
  g_free(p->line);                    /* free duplicated text                    */
  g_free(p);                          /* free payload struct                     */

  return G_SOURCE_REMOVE;             /* one-shot idle callback                  */
}

/* Deliver a line into the console pipeline (console -> buffer via idle).         */
static void
console_sink(void *user, const char *line, gboolean is_err)
{
  (void)is_err;                       /* Currently unused; future styling hook   */

  UmiOutputConsole *c = (UmiOutputConsole *)user;  /* back to console instance  */
  if (!c || !line) return;            /* guard against NULLs                    */

  /* Marshal append onto the GTK main loop safely.                                */
  IdleAppend *payload = g_new(IdleAppend, 1);       /* allocate payload         */
  payload->self = c;                                  /* set console            */
  payload->line = g_strdup(line);                     /* duplicate text         */

  /* Priority: G_PRIORITY_DEFAULT ensures reasonable scheduling.                  */
  g_idle_add_full(G_PRIORITY_DEFAULT, idle_append_cb, payload, NULL);
}

/*-----------------------------------------------------------------------------
 * PUBLIC API IMPLEMENTATION
 *---------------------------------------------------------------------------*/

/* Create a new console.
 * If `opt_buf` is NULL, we create a new GtkTextBuffer and mark created_buf=TRUE.
 * We also construct the internal GtkTextView and GtkScrolledWindow so callers
 * can immediately pack the widget returned by umi_output_console_widget().
 */
UmiOutputConsole *
umi_output_console_new(GtkTextBuffer *opt_buf)
{
  /* Allocate and zero-init the console object on the heap.                       */
  UmiOutputConsole *c = g_new0(UmiOutputConsole, 1);  /* zeroed memory block    */

  /* Buffer selection: use caller’s buffer if provided, otherwise create one.     */
  if (opt_buf) {                         /* case: caller supplies buffer         */
    c->buf = opt_buf;                    /* store pointer; we DO NOT own it      */
    c->created_buf = FALSE;              /* reflect ownership policy             */
  } else {                               /* case: no buffer supplied             */
    c->buf = gtk_text_buffer_new(NULL);  /* create default buffer (no tags)      */
    c->created_buf = TRUE;               /* mark as created by us                */
    /* NOTE: We will not explicitly unref this later because the TextView takes
     * a reference; the view’s lifetime and the GTK container graph manage it.   */
  }

  /* Optional ANSI helper (no-op stub for now).                                   */
  c->ansi = umi_ansi_new();              /* may return NULL                      */

  /* Initialize the downstream chain so external code can hook if needed.         */
  c->chain.sink = console_sink;          /* direct to our buffer                 */
  c->chain.user = c;                     /* pass console as user data            */

  /* ---------------------- NEW: Build the internal widget tree ----------------- */

  /* Create a TextView bound to our buffer so text appears immediately.           */
  GtkWidget *tv = gtk_text_view_new_with_buffer(c->buf);  /* view bound to buf  */

  /* Configure the view for console-like behavior.                                */
  gtk_text_view_set_monospace(GTK_TEXT_VIEW(tv), TRUE);   /* fixed-width font   */
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tv), GTK_WRAP_CHAR); /* wrap lines  */
  gtk_text_view_set_editable(GTK_TEXT_VIEW(tv), FALSE);   /* read-only console  */
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(tv), FALSE); /* hide caret     */

  /* Create a scrolled container and put the TextView inside it.                  */
  GtkWidget *scr = gtk_scrolled_window_new();            /* GTK4 no adjustments */
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scr),
                                 GTK_POLICY_AUTOMATIC,   /* horizontal policy   */
                                 GTK_POLICY_AUTOMATIC);  /* vertical policy     */
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scr), tv); /* set child     */

  /* Save the widgets so umi_output_console_widget() can return them.             */
  c->view   = tv;                          /* store the text view pointer       */
  c->widget = scr;                         /* store the scroller as our widget  */

  /* Return the fully initialized console instance.                               */
  return c;                                /* caller owns the console handle     */
}

/* Return the downstream sink chain (so other modules can feed lines in).         */
UmiOutChain *
umi_output_console_chain(UmiOutputConsole *c)
{
  if (!c) return NULL;                  /* guard                                  */
  return &c->chain;                     /* pointer to stable, embedded member     */
}

/* Return the GtkTextBuffer used by this console (non-owning).                    */
GtkTextBuffer *
umi_output_console_buffer(UmiOutputConsole *c)
{
  if (!c) return NULL;                  /* guard                                  */
  return c->buf;                        /* buffer is owned by the view            */
}

/* Destroy the console and its internal helpers.                                  */
void
umi_output_console_free(UmiOutputConsole *c)
{
  if (!c) return;                       /* guard                                  */

  /* Drop ANSI helper if present.                                                 */
  if (c->ansi) {
    umi_ansi_free(c->ansi);             /* free helper                            */
    c->ansi = NULL;                     /* avoid dangling pointer                 }
  }

  /* NEW: Unref our widget if we still hold a reference.
   * Safe in GTK4: if the widget is packed somewhere else, this just decrements
   * our ref; the container still owns it. If it’s not packed, it will be freed.
   */
  if (c->widget) {
    g_object_unref(c->widget);          /* release scrolled window                */
    c->widget = NULL;                   /* avoid dangling pointer                 */
  }
  c->view = NULL;                       /* child is owned by scroller; clear ptr  */

  /* IMPORTANT: Do not unref the buffer explicitly. The TextView owns a ref and
   * will release it when the widget hierarchy is destroyed. If the buffer was
   * supplied by the caller (created_buf==FALSE), we certainly must not unref it.
   */

  g_free(c);                            /* finally free the console instance      */
}

/* Append a line (thread-safe).                                                    */
void
umi_output_console_append_line(UmiOutputConsole *c, const char *line)
{
  if (!c || !line) return;              /* guard                                  */

  /* Prepare a payload for the idle callback to run on the GTK main loop.         */
  IdleAppend *payload = g_new(IdleAppend, 1); /* allocate payload                */
  payload->self = c;                           /* set console                     */
  payload->line = g_strdup(line);              /* duplicate text for safety       */

  /* Queue the UI update on the main loop.                                        */
  g_idle_add_full(G_PRIORITY_DEFAULT, idle_append_cb, payload, NULL);
}

/* Expose the console widget (GtkScrolledWindow).                                  */
GtkWidget *
umi_output_console_widget(UmiOutputConsole *c)
{
  if (!c) return NULL;                  /* guard                                  */
  return c->widget;                     /* return the scrolled window to pack     */
}
/*--------------------------------- End of file --------------------------------*/