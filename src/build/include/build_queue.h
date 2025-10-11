/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build_queue.h
 * PURPOSE: Queued build/run jobs executed by UmiBuildRunner
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_QUEUE_H
#define UMICOM_BUILD_QUEUE_H

#include <gio/gio.h>
#include "build_runner.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _UmiBuildQueue UmiBuildQueue;

/* Create a queue bound to an output pane (may be NULL for silent mode). */
UmiBuildQueue *umi_build_queue_new(UmiOutputPane *out);

/* Enqueue a job: cwd + argv (NULL-terminated). The queue takes a deep copy. */
void umi_build_queue_push(UmiBuildQueue *q, const char *cwd, char * const *argv);

/* Start processing if idle; returns FALSE on immediate error. */
gboolean umi_build_queue_start(UmiBuildQueue *q);

/* Stop current job (if any) and clear pending jobs. */
void umi_build_queue_stop(UmiBuildQueue *q);

/* Destroy queue and free resources. */
void umi_build_queue_free(UmiBuildQueue *q);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UMICOM_BUILD_QUEUE_H */
