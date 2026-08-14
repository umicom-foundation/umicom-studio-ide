/* Umicom Studio IDE | Compiler project bindings v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_COMPILER_PROJECT_BINDING_H
#define UMICOM_STUDIO_COMPILER_PROJECT_BINDING_H
#include "umicom/compiler/compiler.h"
typedef struct UmiStudioCompilerProjectBinding { char workspace_id[UMI_COMPILER_ID_CAPACITY]; char project_root[UMI_COMPILER_PATH_CAPACITY]; char profile_id[UMI_COMPILER_ID_CAPACITY]; UmiCompilerProjectManifest manifest; bool trusted; bool auto_configure; uint64_t revision; } UmiStudioCompilerProjectBinding;
UmiStatus umi_studio_compiler_project_binding_init(UmiStudioCompilerProjectBinding *binding,const char *workspace_id,const char *project_root,const char *profile_id,const UmiCompilerProjectManifest *manifest);
UmiStatus umi_studio_compiler_project_binding_validate(const UmiStudioCompilerProjectBinding *binding,char *out_reason,size_t capacity);
#endif
