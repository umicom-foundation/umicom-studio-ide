/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/profiler.h
 * PURPOSE: Timing utilities (elapsed microseconds/milliseconds)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROFILER_H
#define UMICOM_PROFILER_H

#include <glib.h>

typedef struct {
  gint64 t0_us;
} UmiTimer;

static inline UmiTimer umi_timer_start(void){
  UmiTimer t; t.t0_us = g_get_monotonic_time(); return t;
}

static inline gint64 umi_timer_elapsed_us(const UmiTimer *t){
  return t ? (g_get_monotonic_time() - t->t0_us) : 0;
}

static inline double umi_timer_elapsed_ms(const UmiTimer *t){
  return (double)umi_timer_elapsed_us(t) / 1000.0;
}

#endif /* UMICOM_PROFILER_H */
