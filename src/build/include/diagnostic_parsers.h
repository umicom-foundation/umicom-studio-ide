/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/diagnostic_parsers.h
 * PURPOSE: Parsers for common compiler diagnostics
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DIAGNOSTIC_PARSERS_H
#define UMICOM_DIAGNOSTIC_PARSERS_H

#include <glib.h>
#include "../../panes/problems/include/problem_list.h"

typedef struct {
  gchar *file;
  int line;
  int col;
  UmiDiagSeverity sev;
  gchar *msg;
} UmiDiagParseResult;

/* Parse GCC/Clang style: file:line:col: error|warning|note: message */
gboolean umi_diag_parse_gcc_clang(const char *s, UmiDiagParseResult *out);

/* Parse MSVC style: file(line,col): error C1234: message */
gboolean umi_diag_parse_msvc(const char *s, UmiDiagParseResult *out);

#endif /* UMICOM_DIAGNOSTIC_PARSERS_H */
