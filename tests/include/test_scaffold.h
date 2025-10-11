/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/test_scaffold.h
 * PURPOSE: Minimal test registration and runner
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_SCAFFOLD_H
#define UMICOM_TEST_SCAFFOLD_H

#include <glib.h>

typedef gboolean (*UmiTestFn)(void);

typedef struct {
  GPtrArray *names;
  GPtrArray *fns; /* of UmiTestFn */
} UmiTests;

UmiTests *umi_tests_new(void);
void      umi_tests_add(UmiTests *t, const char *name, UmiTestFn fn);
int       umi_tests_run(UmiTests *t); /* returns number of failures */
void      umi_tests_free(UmiTests *t);

#endif /* UMICOM_TEST_SCAFFOLD_H */
