/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/test_scaffold.c
 * PURPOSE: Implementation of minimal test scaffold
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/test_scaffold.h"

UmiTests *umi_tests_new(void){
  UmiTests *t = g_new0(UmiTests,1);
  t->names = g_ptr_array_new_with_free_func(g_free);
  t->fns   = g_ptr_array_new();
  return t;
}

void umi_tests_add(UmiTests *t, const char *name, UmiTestFn fn){
  if(!t || !name || !fn) return;
  g_ptr_array_add(t->names, g_strdup(name));
  g_ptr_array_add(t->fns, (gpointer)fn);
}

int umi_tests_run(UmiTests *t){
  if(!t) return 0;
  int fails = 0;
  for(guint i=0;i<t->fns->len;i++){
    UmiTestFn fn = (UmiTestFn)t->fns->pdata[i];
    const char *name = (const char*)t->names->pdata[i];
    gboolean ok = fn();
    if(!ok){ g_printerr("FAIL: %s\n", name); fails++; }
    else    { g_print  ("PASS: %s\n", name); }
  }
  return fails;
}

void umi_tests_free(UmiTests *t){
  if(!t) return;
  g_ptr_array_free(t->names, TRUE);
  g_ptr_array_free(t->fns, TRUE);
  g_free(t);
}
