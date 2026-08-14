/* Umicom Studio IDE | Compiler export v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_COMPILER_EXPORT_V2_H
#define UMICOM_STUDIO_COMPILER_EXPORT_V2_H
#include "umicom/compiler/compiler.h"
UmiStatus umi_studio_compiler_export_summary_v2(const UmiCompilerCentreSnapshot *snapshot,char *out_text,size_t capacity);
UmiStatus umi_studio_compiler_export_database_v2(const UmiCompilerCentre *centre,char *out_json,size_t capacity);
#endif
