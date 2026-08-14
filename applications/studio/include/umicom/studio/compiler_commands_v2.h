/* Umicom Studio IDE | Compiler Centre commands v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_COMPILER_COMMANDS_V2_H
#define UMICOM_STUDIO_COMPILER_COMMANDS_V2_H
#include <stddef.h>
#include <stdbool.h>
typedef struct UmiStudioCompilerCommandV2 { const char *command_id; const char *title; const char *category; const char *default_key; bool requires_project; bool requires_provider; bool destructive; } UmiStudioCompilerCommandV2;
size_t umi_studio_compiler_commands_v2_count(void);
const UmiStudioCompilerCommandV2 *umi_studio_compiler_commands_v2_at(size_t index);
const UmiStudioCompilerCommandV2 *umi_studio_compiler_commands_v2_find(const char *command_id);
#endif
