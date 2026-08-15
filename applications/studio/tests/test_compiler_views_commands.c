/* Umicom Studio Batch 58 test | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/compiler_commands.h"
#include <string.h>
#include "umicom/studio/compiler_views.h"
#include <assert.h>
int main(void) { const UmiStudioCompilerCommand *profile; assert(umi_studio_compiler_views_count() >= 8U); assert(umi_studio_compiler_views_find("compiler.interop") != NULL); assert(umi_studio_compiler_commands_count() >= 10U); assert(umi_studio_compiler_commands_find("compiler.build") != NULL); profile = umi_studio_compiler_commands_find("compiler.profile.select"); assert(profile != NULL); assert(strcmp(profile->default_key, "Ctrl+Alt+Shift+P") == 0); return 0; }
