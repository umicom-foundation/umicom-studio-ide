/* Umicom Studio IDE | Compiler project templates v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_COMPILER_TEMPLATES_V2_H
#define UMICOM_STUDIO_COMPILER_TEMPLATES_V2_H
#include "umicom/compiler/compiler.h"
typedef struct UmiStudioCompilerTemplateV2 { const char *template_id; const char *name; const char *description; const char *relative_path; uint32_t language_mask; bool polyglot; bool requires_umicc; } UmiStudioCompilerTemplateV2;
size_t umi_studio_compiler_templates_v2_count(void);
const UmiStudioCompilerTemplateV2 *umi_studio_compiler_templates_v2_at(size_t index);
const UmiStudioCompilerTemplateV2 *umi_studio_compiler_templates_v2_find(const char *template_id);
#endif
