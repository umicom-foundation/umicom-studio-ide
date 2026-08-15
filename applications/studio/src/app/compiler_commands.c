/* Umicom Studio IDE | Compiler Centre commands v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/compiler_commands.h"
#include <string.h>
static const UmiStudioCompilerCommand COMMANDS[] = {
    {"compiler.detect","Detect Compilers","Compiler","Ctrl+Shift+Alt+D",false,false,false},
    {"compiler.profile.select","Select Compiler Profile","Compiler","Ctrl+Alt+Shift+P",false,true,false},
    {"compiler.project.configure","Configure Polyglot Project","Build","Ctrl+Shift+C",true,true,false},
    {"compiler.build","Build Active Project","Build","Ctrl+Shift+B",true,true,false},
    {"compiler.rebuild","Rebuild Active Project","Build","Ctrl+Alt+B",true,true,true},
    {"compiler.check","Check Without Linking","Build","Ctrl+Shift+K",true,true,false},
    {"compiler.database.export","Export Compilation Database","Compiler","",true,true,false},
    {"compiler.lock.refresh","Refresh Compiler Lockfile","Compiler","",true,true,false},
    {"compiler.interop.validate","Validate ABI Boundaries","Compiler","",true,true,false},
    {"compiler.umicc.open","Open Umicc Integration","Compiler","",false,false,false},
    {"compiler.output.clear","Clear Compiler Output","View","",false,false,true}
};
size_t umi_studio_compiler_commands_count(void) { return sizeof(COMMANDS) / sizeof(COMMANDS[0]); }
const UmiStudioCompilerCommand *umi_studio_compiler_commands_at(size_t index) { return index < umi_studio_compiler_commands_count() ? &COMMANDS[index] : NULL; }
const UmiStudioCompilerCommand *umi_studio_compiler_commands_find(const char *command_id) { size_t index; if (command_id == NULL) return NULL; for (index = 0U; index < umi_studio_compiler_commands_count(); ++index) if (strcmp(COMMANDS[index].command_id,command_id) == 0) return &COMMANDS[index]; return NULL; }
