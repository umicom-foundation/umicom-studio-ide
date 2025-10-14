/*-----------------------------------------------------------------------------
* Umicom Studio IDE
* File: src/build/include/build_runner.h
*
* PURPOSE:
*   Process runner for build tools. Exposes a small API to run an external
*   command with environment/cwd, and route its output to a UmiOutputSink.
*
* DESIGN:
*   - Keeps UmiBuildRunner opaque; ownership via new/free.
*   - Decoupled output via umi_output_sink.h (no UI types here).
*   - Back-compat shims for legacy call sites (setter overload + 7-arg run).
*
* Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
*---------------------------------------------------------------------------*/
#ifndef BUILD_RUNNER_H
#define BUILD_RUNNER_H

#include <glib.h>
#include "umi_output_sink.h"

G_BEGIN_DECLS

typedef struct UmiBuildRunner UmiBuildRunner;

/* Canonical APIs */
UmiBuildRunner *umi_build_runner_new(void);
void            umi_build_runner_free(UmiBuildRunner *br);
void            umi_build_runner_set_sink(UmiBuildRunner *br, UmiOutputSink *sink);

gboolean        umi_build_runner_run(UmiBuildRunner        *br,
                                     const char            *cwd,
                                     const char            *exe,
                                     const char * const     argv[],   /* const-correct */
                                     const char * const     envp[],   /* const-correct */
                                     gboolean               merge_stderr);

/* -------------------------------------------------------------------------- */
/* Back-compat: accept legacy (cb,user) setter and a 7-arg run() call         */
/* -------------------------------------------------------------------------- */
static inline void
umi_build_runner_set_sink_from_cb(UmiBuildRunner *br,
                                  UmiOutputLineFn line_cb,
                                  void           *user)
{
  UmiOutputSink *sink = umi_output_sink_new(line_cb, NULL, user);
  umi_build_runner_set_sink(br, sink);
}

/* Variadic macro overload shim for set_sink */
#define UMI_BR_SINK_PICK(_1,_2,_3,NAME,...) NAME
#define umi_build_runner_set_sink(...) \
  UMI_BR_SINK_PICK(__VA_ARGS__, umi_build_runner_set_sink_from_cb, umi_build_runner_set_sink)(__VA_ARGS__)

/* Variadic macro overload shim for run() — drop extra trailing arg if present */
static inline gboolean
umi_build_runner_run_compat7(UmiBuildRunner        *br,
                             const char            *cwd,
                             const char            *exe,
                             const char * const     argv[],
                             const char * const     envp[],
                             gboolean               merge_stderr,
                             void                  *ignored /* named to avoid warnings */)
{
  (void)ignored;
  return umi_build_runner_run(br, cwd, exe, argv, envp, merge_stderr);
}

#define UMI_BR_RUN_PICK(_1,_2,_3,_4,_5,_6,_7,NAME,...) NAME
#define umi_build_runner_run(...) \
  UMI_BR_RUN_PICK(__VA_ARGS__, umi_build_runner_run_compat7, umi_build_runner_run)(__VA_ARGS__)

G_END_DECLS
#endif /* BUILD_RUNNER_H */
/*  END OF FILE */