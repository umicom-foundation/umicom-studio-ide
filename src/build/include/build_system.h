/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build_system.h
 * PURPOSE: Build system command templates per OS
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_SYSTEM_H
#define UMICOM_BUILD_SYSTEM_H

#include <glib.h>

typedef enum { UMI_TOOL_NINJA, UMI_TOOL_MAKE, UMI_TOOL_MSBUILD, UMI_TOOL_CUSTOM } UmiTool;

typedef struct {
  UmiTool tool;
  gchar *build_cmd;
  gchar *run_cmd;
  gchar *test_cmd;
} UmiBuildSys;

/* Detect a likely build tool in 'root' (presence-based heuristic). */
UmiBuildSys *umi_buildsys_detect(const char *root);

/* Override with custom commands. */
void umi_buildsys_set(UmiBuildSys *bs, const char *build, const char *run, const char *test);

/* Expand to argv (NULL-terminated). Caller frees GPtrArray*. */
GPtrArray *umi_buildsys_build_argv(const UmiBuildSys *bs);
GPtrArray *umi_buildsys_run_argv(const UmiBuildSys *bs);
GPtrArray *umi_buildsys_test_argv(const UmiBuildSys *bs);

void umi_buildsys_free(UmiBuildSys *bs);

#endif /* UMICOM_BUILD_SYSTEM_H */
