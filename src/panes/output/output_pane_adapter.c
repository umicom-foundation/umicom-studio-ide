/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/panes/output/output_pane_adapter.c
 * PURPOSE:
 *   Tiny glue that adapts/embeds the output console (UmiOutputConsole) into
 *   the Output pane area. It constructs the console and exposes a widget for
 *   packing into the UI.
 *
 * DESIGN CHOICES:
 *   - Self-contained: includes only its own public header (which in turn pulls
 *     the console header). No cross-folder deps beyond the published API.
 *   - Ownership: the adapter owns the UmiOutputConsole instance; the caller
 *     owns the returned GtkWidget (packing only — do not destroy directly).
 *   - Buffer policy: we pass NULL to umi_output_console_new(), instructing it
 *     to allocate a fresh GtkTextBuffer internally (per the public API).
 *     This matches the current console contract where the widget/view usually
 *     owns the buffer’s lifetime and the console won’t unref it on free.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/output_pane_adapter.h"  /* Brings in output_console.h too. */

/*-----------------------------------------------------------------------------
 * umi_output_pane_adapter_new
 *
 * Construct a new adapter and its console. We use NULL for the buffer to let
 * the console allocate its own GtkTextBuffer. This aligns with the public API:
 *   UmiOutputConsole *umi_output_console_new(GtkTextBuffer *buf);
 * and avoids the “too few arguments” build error.
 *---------------------------------------------------------------------------*/
UmiOutputPaneAdapter *umi_output_pane_adapter_new(void)
{
  UmiOutputPaneAdapter *a = g_new0(UmiOutputPaneAdapter, 1);

  /* IMPORTANT: The console API expects a GtkTextBuffer* (nullable). Passing
   * NULL here tells it to create a fresh buffer internally. This matches the
   * current header + implementation you provided. */
  a->console = umi_output_console_new(NULL);

  return a;
}

/*-----------------------------------------------------------------------------
 * umi_output_pane_adapter_widget
 *
 * Returns the widget that renders the console. Ownership of the widget is not
 * transferred; pack it into your UI but do not explicitly destroy it — it is
 * managed by the console.
 *---------------------------------------------------------------------------*/
GtkWidget *umi_output_pane_adapter_widget(UmiOutputPaneAdapter *a)
{
  if (!a || !a->console)
    return NULL;

  return umi_output_console_widget(a->console);
}

/*--- end of file ---*/
