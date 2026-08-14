/* Umicom Studio IDE | Compiler project templates v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/compiler_templates.h"
#include <string.h>
static const UmiStudioCompilerTemplate TEMPLATES[] = {
    {"c23-console","C23 Console","Portable C23 console project with strict warnings.","compiler/c",UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_C),false,false},
    {"cpp23-console","C++23 Console","Modern C++ console project with a native ABI boundary.","compiler/cpp",UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_CPP),false,false},
    {"c-assembly","C and Assembly","C host with an audited native assembly unit.","compiler/assembly",UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_C) | UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_ASSEMBLY),true,false},
    {"rust-console","Rust Console","Rust 2024 console project and Cargo metadata.","compiler/rust",UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_RUST),false,false},
    {"zig-console","Zig Console","Zig console project and native build definition.","compiler/zig",UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_ZIG),false,false},
    {"uai-console","UAI Console","UAI application compiled through Umicc.","compiler/uai",UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_UAI),false,true},
    {"polyglot-console","Polyglot Console","C, C++, Rust, Zig and UAI joined by a stable C ABI.","compiler/polyglot",UINT32_MAX,true,true}
};
size_t umi_studio_compiler_templates_count(void) { return sizeof(TEMPLATES) / sizeof(TEMPLATES[0]); }
const UmiStudioCompilerTemplate *umi_studio_compiler_templates_at(size_t index) { return index < umi_studio_compiler_templates_count() ? &TEMPLATES[index] : NULL; }
const UmiStudioCompilerTemplate *umi_studio_compiler_templates_find(const char *template_id) { size_t index; if (template_id == NULL) return NULL; for (index = 0U; index < umi_studio_compiler_templates_count(); ++index) if (strcmp(TEMPLATES[index].template_id,template_id) == 0) return &TEMPLATES[index]; return NULL; }
