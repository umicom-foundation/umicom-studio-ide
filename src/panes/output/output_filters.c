/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/output_filters.c
 * PURPOSE: Implementation of output filter chain
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/output_filters.h"
#include <string.h>
#include <ctype.h>

/* Opaque data; currently only holds the target buffer. */
struct _UmiAnsi {
  GtkTextBuffer *buf; /* not owned */
};

UmiAnsi *umi_ansi_new(GtkTextBuffer *buf) {
  g_return_val_if_fail(GTK_IS_TEXT_BUFFER(buf), NULL);
  UmiAnsi *a = g_new0(UmiAnsi, 1);
  a->buf = buf;
  return a;
}

/* Cheap ANSI stripper: removes ESC[ ... m and ESC[ ... K etc. */
static void strip_ansi_into(GString *dst, const char *src) {
  const unsigned char *p = (const unsigned char *)src;
  while (*p) {
    if (*p == 0x1B && p[1] == '[') {
      /* Skip ESC '[' ... until a letter (final byte of CSI). */
      p += 2;
      while (*p && !((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z'))) p++;
      if (*p) p++; /* consume final letter */
      continue;
    }
    g_string_append_c(dst, (char)*p++);
  }
}

void umi_ansi_append_line(UmiAnsi *a, const char *line) {
  if (!a || !a->buf || !line) return;

  g_autoptr(GString) clean = g_string_new(NULL);
  strip_ansi_into(clean, line);

  /* Ensure a trailing newline once. */
  gboolean add_nl = clean->len == 0 || clean->str[clean->len - 1] != '\n';
  GtkTextIter end;
  gtk_text_buffer_get_end_iter(a->buf, &end);
  gtk_text_buffer_insert(a->buf, &end, clean->str, (gint)clean->len);
  if (add_nl)
    gtk_text_buffer_insert(a->buf, &end, "\n", 1);
}

void umi_ansi_free(UmiAnsi *a) {
  g_free(a);
}
/*--- end of file ---*/