/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build_system.c
 * PURPOSE: Implementation of build system detection and argv expansion
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "build_system.h"

static gchar *dup(const char *s){ return g_strdup(s?s:""); }

UmiBuildSys *umi_buildsys_detect(const char *root){
  UmiBuildSys *b = g_new0(UmiBuildSys,1);
#ifdef G_OS_WIN32
  if(g_file_test("build.ninja", G_FILE_TEST_EXISTS)){ b->tool = UMI_TOOL_NINJA; b->build_cmd = dup("ninja"); b->run_cmd = dup("ninja run"); b->test_cmd = dup("ninja test"); }
  else if(g_file_test("Makefile", G_FILE_TEST_EXISTS)){ b->tool = UMI_TOOL_MAKE; b->build_cmd = dup("mingw32-make -j"); b->run_cmd = dup("mingw32-make run"); b->test_cmd = dup("mingw32-make test"); }
  else { b->tool = UMI_TOOL_MSBUILD; b->build_cmd = dup("msbuild /m"); b->run_cmd = dup("build\\app.exe"); b->test_cmd = dup("ctest"); }
#else
  if(g_file_test("build.ninja", G_FILE_TEST_EXISTS)){ b->tool = UMI_TOOL_NINJA; b->build_cmd = dup("ninja"); b->run_cmd = dup("ninja run"); b->test_cmd = dup("ninja test"); }
  else if(g_file_test("Makefile", G_FILE_TEST_EXISTS)){ b->tool = UMI_TOOL_MAKE; b->build_cmd = dup("make -j"); b->run_cmd = dup("make run"); b->test_cmd = dup("make test"); }
  else { b->tool = UMI_TOOL_CUSTOM; b->build_cmd = dup("sh -lc 'echo build'"); b->run_cmd = dup("sh -lc './app'"); b->test_cmd = dup("sh -lc 'ctest'"); }
#endif
  return b;
}

void umi_buildsys_set(UmiBuildSys *bs, const char *build, const char *run, const char *test){
  if(!bs) return;
  g_free(bs->build_cmd); g_free(bs->run_cmd); g_free(bs->test_cmd);
  bs->build_cmd = dup(build); bs->run_cmd = dup(run); bs->test_cmd = dup(test);
}

static GPtrArray *split_argv(const char *cmd){
  GPtrArray *a = g_ptr_array_new_with_free_func(g_free);
  if(!cmd){ g_ptr_array_add(a, g_strdup("true")); g_ptr_array_add(a,NULL); return a; }
  gchar **t = g_strsplit(cmd, " ", -1);
  for(gchar **p=t; p && *p; ++p) if(**p) g_ptr_array_add(a, g_strdup(*p));
  g_strfreev(t);
  g_ptr_array_add(a,NULL);
  return a;
}

GPtrArray *umi_buildsys_build_argv(const UmiBuildSys *bs){ return split_argv(bs?bs->build_cmd:NULL); }
GPtrArray *umi_buildsys_run_argv  (const UmiBuildSys *bs){ return split_argv(bs?bs->run_cmd:NULL);   }
GPtrArray *umi_buildsys_test_argv (const UmiBuildSys *bs){ return split_argv(bs?bs->test_cmd:NULL);  }

void umi_buildsys_free(UmiBuildSys *b){
  if(!b) return;
  g_free(b->build_cmd); g_free(b->run_cmd); g_free(b->test_cmd);
  g_free(b);
}
