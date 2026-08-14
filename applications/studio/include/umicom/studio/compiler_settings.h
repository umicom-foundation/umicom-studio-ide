/* Umicom Studio IDE | Compiler settings v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_COMPILER_SETTINGS_H
#define UMICOM_STUDIO_COMPILER_SETTINGS_H
#include "umicom/compiler/compiler.h"
typedef struct UmiStudioCompilerSettings { char default_profile[UMI_COMPILER_ID_CAPACITY]; char build_directory[UMI_COMPILER_PATH_CAPACITY]; char compile_database_path[UMI_COMPILER_PATH_CAPACITY]; bool auto_detect; bool auto_save_lockfile; bool reveal_diagnostics; bool parallel_build; uint32_t parallel_jobs; uint64_t revision; } UmiStudioCompilerSettings;
void umi_studio_compiler_settings_defaults(UmiStudioCompilerSettings *settings);
UmiStatus umi_studio_compiler_settings_validate(const UmiStudioCompilerSettings *settings,char *out_reason,size_t capacity);
#endif
