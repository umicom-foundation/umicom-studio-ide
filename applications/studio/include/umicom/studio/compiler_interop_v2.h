/* Umicom Studio IDE | Polyglot interoperability view v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_COMPILER_INTEROP_V2_H
#define UMICOM_STUDIO_COMPILER_INTEROP_V2_H
#include "umicom/compiler/compiler.h"
typedef struct UmiStudioCompilerInteropSummaryV2 { size_t units; size_t boundaries; size_t required_boundaries; size_t stable_boundaries; size_t incompatible_boundaries; bool buildable; char headline[UMI_COMPILER_TEXT_CAPACITY]; } UmiStudioCompilerInteropSummaryV2;
UmiStatus umi_studio_compiler_interop_summarise_v2(const UmiInteropGraph *graph,UmiStudioCompilerInteropSummaryV2 *out_summary);
#endif
