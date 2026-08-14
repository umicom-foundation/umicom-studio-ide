/* Umicom Studio IDE | Compiler Centre views v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/compiler_views.h"
#include <string.h>
static const UmiStudioCompilerView VIEWS[] = {
    {"compiler.overview","Compiler Overview","applications-engineering","Providers, targets, profiles and project readiness.",UMI_STUDIO_COMPILER_VIEW_PRIMARY,false,true,false},
    {"compiler.providers","Compiler Providers","preferences-system","GCC, Clang, MSVC, Rust, Zig, UAI and Umicc providers.",UMI_STUDIO_COMPILER_VIEW_SECONDARY,true,true,false},
    {"compiler.build-graph","Build Graph","view-grid","Ordered cross-language compile and link steps.",UMI_STUDIO_COMPILER_VIEW_PRIMARY,true,true,true},
    {"compiler.interop","Interop Boundaries","network-workgroup","Stable ABI contracts between language units.",UMI_STUDIO_COMPILER_VIEW_PRIMARY,true,true,true},
    {"compiler.diagnostics","Compiler Problems","dialog-warning","Normalised compiler diagnostics with source navigation.",UMI_STUDIO_COMPILER_VIEW_BOTTOM,true,true,false},
    {"compiler.output","Compiler Output","utilities-terminal","Raw compiler, linker and Umicc output channels.",UMI_STUDIO_COMPILER_VIEW_BOTTOM,true,true,true},
    {"compiler.database","Compilation Database","x-office-spreadsheet","Polyglot compile commands and export preview.",UMI_STUDIO_COMPILER_VIEW_PRIMARY,true,true,false},
    {"compiler.templates","Project Templates","document-new","Language and polyglot project starters.",UMI_STUDIO_COMPILER_VIEW_SECONDARY,true,true,false}
};
size_t umi_studio_compiler_views_count(void) { return sizeof(VIEWS) / sizeof(VIEWS[0]); }
const UmiStudioCompilerView *umi_studio_compiler_views_at(size_t index) { return index < umi_studio_compiler_views_count() ? &VIEWS[index] : NULL; }
const UmiStudioCompilerView *umi_studio_compiler_views_find(const char *view_id) { size_t index; if (view_id == NULL) return NULL; for (index = 0U; index < umi_studio_compiler_views_count(); ++index) if (strcmp(VIEWS[index].view_id,view_id) == 0) return &VIEWS[index]; return NULL; }
