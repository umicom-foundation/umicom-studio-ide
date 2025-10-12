/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/log/status_util.c
 * PURPOSE: Small helper abstraction over GtkStatusbar
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "status_util.h"  /* UmiStatus API */

UmiStatus *
umi_status_new(GtkStatusbar *bar)
{
    g_return_val_if_fail(GTK_IS_STATUSBAR(bar), NULL);         /* Validate input */

    UmiStatus *st = g_new0(UmiStatus, 1);                      /* Allocate zeroed struct */
    st->bar = bar;                                             /* Borrowed UI pointer */
    st->ctx_id = gtk_statusbar_get_context_id(bar, "umicom");  /* Obtain a context id */
    return st;                                                 /* Return wrapper */
}

void
umi_status_push(UmiStatus *st, const char *msg)
{
    if (!st || !st->bar) return;                               /* Nothing to do if not bound */

    /* GtkStatusbar shows the top of a per-context stack; use push()/pop() as needed. */
    if (!msg || !*msg) {
        /* Empty: clear by pushing a blank or popping — push blank keeps behavior simple. */
        gtk_statusbar_push(st->bar, st->ctx_id, "");           /* Show empty text */
    } else {
        gtk_statusbar_push(st->bar, st->ctx_id, msg);          /* Show provided message */
    }
}

void
umi_status_free(UmiStatus *st)
{
    g_free(st);                                                /* Free wrapper (no UI destroy) */
}
/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/git/git_integration.c
 * PURPOSE: Implementation of Git helpers (child process + capture)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/