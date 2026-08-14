/* Umicom Studio Batch 58 test | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/compiler_centre_v2.h"
#include <assert.h>
int main(void) { UmiStudioCompilerCentreV2 *centre = NULL; UmiStudioCompilerCentreSnapshotV2 snapshot; assert(umi_studio_compiler_centre_v2_create(NULL,&centre) == UMI_STATUS_OK); assert(umi_studio_compiler_centre_v2_register_defaults(centre,"gcc","clang","rustc","zig","",NULL) == UMI_STATUS_OK); assert(umi_studio_compiler_centre_v2_activate_view(centre,"compiler.build-graph") == UMI_STATUS_OK); assert(umi_studio_compiler_centre_v2_snapshot(centre,&snapshot) == UMI_STATUS_OK); assert(snapshot.compiler.providers >= 4U); assert(snapshot.compiler.profiles == 1U); umi_studio_compiler_centre_v2_destroy(centre); return 0; }
