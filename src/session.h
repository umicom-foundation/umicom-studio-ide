/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/session.h
 * PURPOSE: Persist last opened file and caret position
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SESSION_H
#define UMICOM_SESSION_H

#include <gtk/gtk.h>

typedef struct {
  gchar *last_file;
  int caret_line;
  int caret_col;
} UmiSession;

/* Load from config/session.json (returns defaults if not present). */
UmiSession *umi_session_load(void);

/* Save to config/session.json. */
gboolean umi_session_save(const UmiSession *s);

/* Free. */
void umi_session_free(UmiSession *s);

#endif /* UMICOM_SESSION_H */
