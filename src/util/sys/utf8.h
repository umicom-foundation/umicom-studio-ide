/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/utf8.h
 * PURPOSE: UTF-8 helpers (case-insensitive contains)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UTF8_H
#define UMICOM_UTF8_H

#include <glib.h>

/* Case-insensitive substring check for UTF-8 strings.
 * Returns TRUE if 'haystack' contains 'needle' (both UTF-8). */
gboolean umi_utf8_icontains(const char *haystack, const char *needle);

#endif /* UMICOM_UTF8_H */
