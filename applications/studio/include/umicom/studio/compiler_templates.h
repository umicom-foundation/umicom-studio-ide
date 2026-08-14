/* Umicom Studio IDE | Compiler project templates v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_COMPILER_TEMPLATES_H
#define UMICOM_STUDIO_COMPILER_TEMPLATES_H
#include "umicom/compiler/compiler.h"
typedef struct UmiStudioCompilerTemplate { const char *template_id; const char *name; const char *description; const char *relative_path; uint32_t language_mask; bool polyglot; bool requires_umicc; } UmiStudioCompilerTemplate;
size_t umi_studio_compiler_templates_count(void);
const UmiStudioCompilerTemplate *umi_studio_compiler_templates_at(size_t index);
const UmiStudioCompilerTemplate *umi_studio_compiler_templates_find(const char *template_id);
#endif
