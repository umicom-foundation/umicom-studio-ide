/* Umicom Studio Batch 58 test | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/compiler_templates.h"
#include <assert.h>
int main(void) { const UmiStudioCompilerTemplate *poly = umi_studio_compiler_templates_find("polyglot-console"); assert(umi_studio_compiler_templates_count() >= 7U); assert(poly != NULL); assert(poly->polyglot); assert(poly->requires_umicc); return 0; }
