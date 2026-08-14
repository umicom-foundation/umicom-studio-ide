/* Umicom Studio Batch 58 test | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/compiler_commands_v2.h"
#include "umicom/studio/compiler_views_v2.h"
#include <assert.h>
int main(void) { assert(umi_studio_compiler_views_v2_count() >= 8U); assert(umi_studio_compiler_views_v2_find("compiler.interop") != NULL); assert(umi_studio_compiler_commands_v2_count() >= 10U); assert(umi_studio_compiler_commands_v2_find("compiler.build") != NULL); return 0; }
