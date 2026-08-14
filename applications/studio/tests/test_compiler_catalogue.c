/* Umicom Studio Batch 58 test | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/compiler_catalogue.h"
#include <assert.h>
int main(void) { UmiCompilerRegistry registry; UmiCompilerProvider gcc = umi_compiler_gcc_provider("gcc",""); UmiStudioCompilerCatalogue catalogue; umi_compiler_registry_init(&registry); assert(umi_compiler_registry_register(&registry,&gcc) == UMI_STATUS_OK); assert(umi_studio_compiler_catalogue_project(&registry,"gcc",&catalogue) == UMI_STATUS_OK); assert(catalogue.count == 1U); assert(catalogue.rows[0].selected); return 0; }
