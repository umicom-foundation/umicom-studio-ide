/* Umicom Studio Batch 58 test | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/compiler_diagnostics_v2.h"
#include <assert.h>
#include <string.h>
int main(void) { UmiCompilerDiagnosticSet set = {0}; UmiCompilerDiagnostic diagnostic = {0}; UmiStudioCompilerDiagnosticFilterV2 filter = {0}; UmiStudioCompilerDiagnosticSummaryV2 summary; (void)strcpy(diagnostic.file,"main.c"); (void)strcpy(diagnostic.message,"expected expression"); diagnostic.severity = UMI_COMPILER_DIAGNOSTIC_ERROR; assert(umi_compiler_diagnostic_set_add(&set,&diagnostic) == UMI_STATUS_OK); filter.minimum_severity = UMI_COMPILER_DIAGNOSTIC_WARNING; assert(umi_studio_compiler_diagnostics_summarise_v2(&set,&filter,&summary) == UMI_STATUS_OK); assert(summary.errors == 1U); return 0; }
