/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/panes/output/include/output_console.h
 * PURPOSE:
 *   Tiny output-console adapter for piping process/log lines into a GtkTextBuffer,
 *   with zero cross-folder deps. Exposes a minimal "chain" handle so upstream
 *   code can push lines without knowing console internals.
 *
 * DESIGN CHOICES:
 *   - Self-contained: includes only GLib + GTK headers and its own public API.
 *   - No globals: lifetime is owned by the caller that creates/free's the console.
 *   - Thread-safe enqueue: appends are marshalled to GTK main loop via g_idle_add().
 *   - No ANSI parsing here (kept lean). If you add color later, do it behind this API.
 *   - Minimal chain object (UmiOutChain): carries a sink callback + user data; the
 *     callback appends lines into the console buffer on the GTK thread.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OUTPUT_CONSOLE_H
#define UMICOM_OUTPUT_CONSOLE_H

#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

/*-----------------------------------------------------------------------------
 * Public opaque console type
 *---------------------------------------------------------------------------*/
typedef struct _UmiOutputConsole UmiOutputConsole;

/*-----------------------------------------------------------------------------
 * Minimal "chain" holder used by upstream pipeline code.
 *
 * Rationale:
 *   - Some older parts of the project used a type named UmiOutChain. To avoid
 *     cross-folder coupling, we define a small local struct that simply carries
 *     a function pointer (sink) and its user data. Upstream can keep a pointer
 *     to this and just call chain->sink(line, chain->user).
 *
 *   - We do NOT depend on any other pipeline headers or typedefs.
 *---------------------------------------------------------------------------*/
typedef struct _UmiOutChain {
  void    (*sink)(const char *line, gpointer user);  /* line consumer            */
  gpointer user;                                     /* context for the sink     */
  void    (*free_user)(gpointer user);               /* optional custom free     */
} UmiOutChain;

/*-----------------------------------------------------------------------------
 * Console API
 *---------------------------------------------------------------------------*/

/**
 * umi_output_console_new:
 * @buf: (nullable): target GtkTextBuffer. If NULL, a fresh buffer is created.
 *
 * Returns a new console wrapper that will append text to @buf.
 * Lifetime: free with umi_output_console_free().
 */
UmiOutputConsole *umi_output_console_new(GtkTextBuffer *buf);

/**
 * umi_output_console_free:
 * @c: console returned by umi_output_console_new()
 *
 * Destroys the console and its internal chain. Does NOT unref the GtkTextBuffer,
 * because ownership typically belongs to the view/widget that displays it.
 */
void umi_output_console_free(UmiOutputConsole *c);


/* Embedding ---------------------------------------------------------------- */

/**
 * umi_output_console_widget:
 * Returns: (transfer none) the scrollable widget to pack into your UI.
 * The widget remains owned by the console; do not unref/destroy directly.
 */
GtkWidget        *umi_output_console_widget(UmiOutputConsole *c);

/** Optional: obtain the internal text buffer for advanced operations. */
GtkTextBuffer    *umi_output_console_buffer(UmiOutputConsole *c);

/* Content operations ------------------------------------------------------- */

/** Append a line of UTF-8 text (a '\n' is added). NULL is ignored. */
void              umi_output_console_append_line(UmiOutputConsole *c,
                                                 const char *line);

/** Clear the console contents. Safe on NULL. */
void              umi_output_console_clear(UmiOutputConsole *c);

/**
 * umi_output_console_buffer:
 * @c: console
 *
 * Returns the GtkTextBuffer used by this console (never NULL).
 * Ownership: not transferred.
 */
GtkTextBuffer *umi_output_console_buffer(UmiOutputConsole *c);

/**
 * umi_output_console_append_line:
 * @c: console
 * @line: UTF-8 line (may be NULL or empty; NULL is ignored)
 *
 * Schedules an append of @line + '\n' onto the GTK main loop safely.
 */
void umi_output_console_append_line(UmiOutputConsole *c, const char *line);

/*-----------------------------------------------------------------------------
 * Chain bridge
 *---------------------------------------------------------------------------*/

/**
 * umi_output_console_chain:
 * @c: console
 *
 * Returns a chain object whose sink appends lines into @c.
 * Ownership: belongs to @c; do NOT free it manually (use umi_out_chain_ref()
 * / umi_out_chain_unref() if you add refcounting later). For now, treat it as
 * non-owning and only pass the pointer around.
 */
UmiOutChain *umi_output_console_chain(UmiOutputConsole *c);

/* (Optional) If external code wants to create/destroy its own chain wrappers,
 * you can expose helpers. For now, only the console owns its chain.
 * Keeping these internal avoids unwanted coupling. */

/*---------------------------------------------------------------------------*/

G_END_DECLS

#endif /* UMICOM_OUTPUT_CONSOLE_H */
/*--- end of file ---*/