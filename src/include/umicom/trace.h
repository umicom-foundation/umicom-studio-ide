/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/trace.h
 * PURPOSE: Simple tracing helpers (begin/end marks)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TRACE_H
#define UMICOM_TRACE_H

#include <glib.h>

typedef struct {
  gchar *name;
  gint64 t0;
} UmiTrace;

static inline UmiTrace umi_trace_begin(const char *name){
  UmiTrace t; t.name = g_strdup(name?name:"trace"); t.t0 = g_get_monotonic_time(); return t;
}
static inline gint64  umi_trace_end(const UmiTrace *t){
  return t? (g_get_monotonic_time() - t->t0) : 0;
}
static inline void    umi_trace_free(UmiTrace *t){ if(!t) return; g_free(t->name); t->name=NULL; }

#endif /* UMICOM_TRACE_H */
