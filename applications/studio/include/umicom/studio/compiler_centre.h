/* Umicom Studio IDE | Compiler and Polyglot Centre v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_COMPILER_CENTRE_H
#define UMICOM_STUDIO_COMPILER_CENTRE_H
#include "umicom/studio/compiler_settings.h"
#include "umicom/studio/toolchain_centre.h"
typedef struct UmiStudioCompilerCentre UmiStudioCompilerCentre;
typedef struct UmiStudioCompilerCentreSnapshot { UmiCompilerCentreSnapshot compiler; UmiStudioCompilerSettings settings; char active_view[UMI_COMPILER_ID_CAPACITY]; bool toolchain_synchronised; bool project_open; uint64_t revision; } UmiStudioCompilerCentreSnapshot;
UmiStatus umi_studio_compiler_centre_create(UmiStudioToolchainCentre *toolchains,UmiStudioCompilerCentre **out_centre);
void umi_studio_compiler_centre_destroy(UmiStudioCompilerCentre *centre);
UmiStatus umi_studio_compiler_centre_register_defaults(UmiStudioCompilerCentre *centre,const char *gcc,const char *clang,const char *rustc,const char *zig,const char *umicc,const char *target);
UmiStatus umi_studio_compiler_centre_sync_toolchain(UmiStudioCompilerCentre *centre);
UmiStatus umi_studio_compiler_centre_open_project(UmiStudioCompilerCentre *centre,const UmiCompilerProjectManifest *manifest);
UmiStatus umi_studio_compiler_centre_activate_view(UmiStudioCompilerCentre *centre,const char *view_id);
UmiStatus umi_studio_compiler_centre_plan_unit(UmiStudioCompilerCentre *centre,const char *project_id,const char *unit_id,UmiCompilerPlan *out_plan);
UmiStatus umi_studio_compiler_centre_snapshot(const UmiStudioCompilerCentre *centre,UmiStudioCompilerCentreSnapshot *out_snapshot);
UmiCompilerCentre *umi_studio_compiler_centre_engine(UmiStudioCompilerCentre *centre);
#endif
