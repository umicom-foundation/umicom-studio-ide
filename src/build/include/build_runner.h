/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/include/build_runner.h
 *
 * PURPOSE:
 *   Launch and supervise an external build/test process with GLib's
 *   GSubprocess APIs, streaming lines to a decoupled UmiOutputSink.
 *
 * DESIGN:
 *   - Opaque UmiBuildRunner handle; implementation is private to .c
 *   - No UI/pane headers included here; this stays in the build module.
 *   - Caller injects an UmiOutputSink instance (strategy pattern).
 *
 * API:
 *   UmiBuildRunner *umi_build_runner_new(void);
 *   void            umi_build_runner_free(UmiBuildRunner *br);
 *   void            umi_build_runner_set_sink(UmiBuildRunner *br,
 *                                             UmiOutputSink   *sink);
 *   gboolean        umi_build_runner_run(UmiBuildRunner *br,
 *                                        char * const   *argv,   // NULL-terminated
 *                                        char * const   *envp,   // may be NULL
 *                                        const char     *cwd,    // may be NULL
 *                                        void (*on_exit)(gpointer user, int code),
 *                                        gpointer        user);
 *   void            umi_build_runner_stop(UmiBuildRunner *br);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_BUILD_RUNNER_H
#define UMI_BUILD_RUNNER_H

#include <glib.h>
#include <gio/gio.h>

#include "umi_output_sink.h" /* decoupled sink */

G_BEGIN_DECLS

typedef struct _UmiBuildRunner UmiBuildRunner; /* opaque */

UmiBuildRunner *umi_build_runner_new(void);
void            umi_build_runner_free(UmiBuildRunner *br);

void            umi_build_runner_set_sink(UmiBuildRunner *br, UmiOutputSink *sink);

gboolean        umi_build_runner_run(UmiBuildRunner *br,
                                     char * const   *argv,   /* NULL-terminated               */
                                     char * const   *envp,   /* optional env (NULL => inherit)*/
                                     const char     *cwd,    /* working dir (NULL => current) */
                                     void (*on_exit)(gpointer user, int code),
                                     gpointer        user);

void            umi_build_runner_stop(UmiBuildRunner *br);

G_END_DECLS

#endif /* UMI_BUILD_RUNNER_H */
