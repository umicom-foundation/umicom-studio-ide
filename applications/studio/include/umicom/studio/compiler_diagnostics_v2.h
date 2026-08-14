/* Umicom Studio IDE | Compiler diagnostics projection v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_COMPILER_DIAGNOSTICS_V2_H
#define UMICOM_STUDIO_COMPILER_DIAGNOSTICS_V2_H
#include "umicom/compiler/compiler.h"
typedef struct UmiStudioCompilerDiagnosticFilterV2 { UmiCompilerDiagnosticSeverity minimum_severity; char file_contains[128U]; char code_contains[64U]; } UmiStudioCompilerDiagnosticFilterV2;
typedef struct UmiStudioCompilerDiagnosticSummaryV2 { size_t visible; size_t notes; size_t warnings; size_t errors; char headline[UMI_COMPILER_TEXT_CAPACITY]; } UmiStudioCompilerDiagnosticSummaryV2;
bool umi_studio_compiler_diagnostic_matches_v2(const UmiCompilerDiagnostic *diagnostic,const UmiStudioCompilerDiagnosticFilterV2 *filter);
UmiStatus umi_studio_compiler_diagnostics_summarise_v2(const UmiCompilerDiagnosticSet *set,const UmiStudioCompilerDiagnosticFilterV2 *filter,UmiStudioCompilerDiagnosticSummaryV2 *out_summary);
#endif
