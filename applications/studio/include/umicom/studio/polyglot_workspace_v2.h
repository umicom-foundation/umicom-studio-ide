/* Umicom Studio IDE | Polyglot workspace v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_POLYGLOT_WORKSPACE_V2_H
#define UMICOM_STUDIO_POLYGLOT_WORKSPACE_V2_H
#include "umicom/compiler/compiler.h"
typedef struct UmiStudioPolyglotWorkspaceV2 { char workspace_id[UMI_COMPILER_ID_CAPACITY]; UmiCompilerProjectManifest manifest; UmiInteropGraph interop; UmiCompilerBuildGraph build; char active_unit[UMI_COMPILER_ID_CAPACITY]; uint32_t language_mask; bool dirty; uint64_t revision; } UmiStudioPolyglotWorkspaceV2;
UmiStatus umi_studio_polyglot_workspace_v2_init(UmiStudioPolyglotWorkspaceV2 *workspace,const char *workspace_id,const UmiCompilerProjectManifest *manifest);
UmiStatus umi_studio_polyglot_workspace_v2_select_unit(UmiStudioPolyglotWorkspaceV2 *workspace,const char *unit_id);
size_t umi_studio_polyglot_workspace_v2_language_count(const UmiStudioPolyglotWorkspaceV2 *workspace);
#endif
