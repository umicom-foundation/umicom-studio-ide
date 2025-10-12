/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/include/build_queue.h
 * PURPOSE: A simple FIFO queue of build tasks managed around UmiBuildRunner.
 * NOTE:    Avoids GUI includes by forward-declaring UmiOutputPane.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_QUEUE_H          /* Include guard start */
#define UMICOM_BUILD_QUEUE_H          /* Mark guard defined */

#include <stddef.h>                   /* size_t for counts/lengths */
#ifdef __cplusplus
extern "C" {
#endif
/* Forward declarations to avoid dragging GUI headers here */
typedef struct _UmiOutputPane UmiOutputPane;   /* Opaque console/output pane */
typedef struct _UmiBuildRunner UmiBuildRunner; /* From build_runner.h */
typedef struct _UmiBuildQueue  UmiBuildQueue;  /* Opaque queue object */

/*-----------------------------------------------------------------------------
 * Lifecycle
 *---------------------------------------------------------------------------*/

/* Create a queue that prints to the output pane (may be NULL for silent). */
UmiBuildQueue *umi_build_queue_new(UmiOutputPane *out);

/* Destroy the queue and its internal resources. Safe on NULL. */
void           umi_build_queue_free(UmiBuildQueue *q);

/*-----------------------------------------------------------------------------
 * Enqueue / Control
 *---------------------------------------------------------------------------*/

/* Add a build command (cmdline + optional working directory). Returns 0 on ok. */
int            umi_build_queue_push(UmiBuildQueue *q,
                                    const char    *cmdline,
                                    const char    *workdir);

/* Start processing queued tasks if idle. */
int          umi_build_queue_start(UmiBuildQueue *q);

/* Abort current task and clear pending items. */
void           umi_build_queue_abort_all(UmiBuildQueue *q);

/* Number of pending tasks (not including the one possibly running). */
size_t         umi_build_queue_size(const UmiBuildQueue *q);

/* Is the queue currently executing a task? (non-zero = yes) */
int            umi_build_queue_is_busy(const UmiBuildQueue *q);

/* Optional: access to underlying runner (do not free/own it). */
UmiBuildRunner *umi_build_queue_runner(UmiBuildQueue *q);

#endif /* UMICOM_BUILD_QUEUE_H */     /* Include guard end */
