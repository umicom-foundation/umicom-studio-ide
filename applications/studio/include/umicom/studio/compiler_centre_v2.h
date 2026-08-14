/* Umicom Studio IDE | Compiler and Polyglot Centre v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_COMPILER_CENTRE_V2_H
#define UMICOM_STUDIO_COMPILER_CENTRE_V2_H
#include "umicom/studio/compiler_settings_v2.h"
#include "umicom/studio/toolchain_centre.h"
typedef struct UmiStudioCompilerCentreV2 UmiStudioCompilerCentreV2;
typedef struct UmiStudioCompilerCentreSnapshotV2 { UmiCompilerCentreSnapshot compiler; UmiStudioCompilerSettingsV2 settings; char active_view[UMI_COMPILER_ID_CAPACITY]; bool toolchain_synchronised; bool project_open; uint64_t revision; } UmiStudioCompilerCentreSnapshotV2;
UmiStatus umi_studio_compiler_centre_v2_create(UmiStudioToolchainCentre *toolchains,UmiStudioCompilerCentreV2 **out_centre);
void umi_studio_compiler_centre_v2_destroy(UmiStudioCompilerCentreV2 *centre);
UmiStatus umi_studio_compiler_centre_v2_register_defaults(UmiStudioCompilerCentreV2 *centre,const char *gcc,const char *clang,const char *rustc,const char *zig,const char *umicc,const char *target);
UmiStatus umi_studio_compiler_centre_v2_sync_toolchain(UmiStudioCompilerCentreV2 *centre);
UmiStatus umi_studio_compiler_centre_v2_open_project(UmiStudioCompilerCentreV2 *centre,const UmiCompilerProjectManifest *manifest);
UmiStatus umi_studio_compiler_centre_v2_activate_view(UmiStudioCompilerCentreV2 *centre,const char *view_id);
UmiStatus umi_studio_compiler_centre_v2_plan_unit(UmiStudioCompilerCentreV2 *centre,const char *project_id,const char *unit_id,UmiCompilerPlan *out_plan);
UmiStatus umi_studio_compiler_centre_v2_snapshot(const UmiStudioCompilerCentreV2 *centre,UmiStudioCompilerCentreSnapshotV2 *out_snapshot);
UmiCompilerCentre *umi_studio_compiler_centre_v2_engine(UmiStudioCompilerCentreV2 *centre);
#endif
