/* Umicom Studio Batch 58 test | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/compiler_centre.h"
#include <assert.h>
int main(void) { UmiStudioCompilerCentre *centre = NULL; UmiStudioCompilerCentreSnapshot snapshot; assert(umi_studio_compiler_centre_create(NULL,&centre) == UMI_STATUS_OK); assert(umi_studio_compiler_centre_register_defaults(centre,"gcc","clang","rustc","zig","",NULL) == UMI_STATUS_OK); assert(umi_studio_compiler_centre_activate_view(centre,"compiler.build-graph") == UMI_STATUS_OK); assert(umi_studio_compiler_centre_snapshot(centre,&snapshot) == UMI_STATUS_OK); assert(snapshot.compiler.providers >= 4U); assert(snapshot.compiler.profiles == 1U); umi_studio_compiler_centre_destroy(centre); return 0; }
