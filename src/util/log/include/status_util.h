/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/log/include/status_util.h
 * PURPOSE: Small helper abstraction over GtkStatusbar
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#pragma once
#ifndef UMICOM_STATUS_UTIL_H
#define UMICOM_STATUS_UTIL_H

#include <gtk/gtk.h>   /* GtkStatusbar, guint */

/* Forward/opaque struct the rest of the code can hold. */
typedef struct _UmiStatus UmiStatus;

struct _UmiStatus {
  GtkStatusbar *bar;   /* Borrowed; owned by UI */
  guint         ctx_id;/* Statusbar context id for the app's messages */
};

/* Create a new UmiStatus bound to @bar. Does not take ownership of @bar. */
UmiStatus *umi_status_new (GtkStatusbar *bar);

/* Push a formatted message to the status bar. NULL/"" clears it. */
void       umi_status_push(UmiStatus *st, const char *msg);

/* Free the wrapper (does not destroy the GtkStatusbar). */
void       umi_status_free(UmiStatus *st);

#endif /* UMICOM_STATUS_UTIL_H */
/* End of src/util/log/include/status_util.h */