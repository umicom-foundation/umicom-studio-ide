/* Umicom Studio IDE | Compiler diagnostics projection v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_COMPILER_DIAGNOSTICS_H
#define UMICOM_STUDIO_COMPILER_DIAGNOSTICS_H
#include "umicom/compiler/compiler.h"
typedef struct UmiStudioCompilerDiagnosticFilter { UmiCompilerDiagnosticSeverity minimum_severity; char file_contains[128U]; char code_contains[64U]; } UmiStudioCompilerDiagnosticFilter;
typedef struct UmiStudioCompilerDiagnosticSummary { size_t visible; size_t notes; size_t warnings; size_t errors; char headline[UMI_COMPILER_TEXT_CAPACITY]; } UmiStudioCompilerDiagnosticSummary;
bool umi_studio_compiler_diagnostic_matches(const UmiCompilerDiagnostic *diagnostic,const UmiStudioCompilerDiagnosticFilter *filter);
UmiStatus umi_studio_compiler_diagnostics_summarise(const UmiCompilerDiagnosticSet *set,const UmiStudioCompilerDiagnosticFilter *filter,UmiStudioCompilerDiagnosticSummary *out_summary);
#endif
