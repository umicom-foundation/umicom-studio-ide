/* Umicom Studio IDE | Compiler Centre commands v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_COMPILER_COMMANDS_H
#define UMICOM_STUDIO_COMPILER_COMMANDS_H
#include <stddef.h>
#include <stdbool.h>
typedef struct UmiStudioCompilerCommand { const char *command_id; const char *title; const char *category; const char *default_key; bool requires_project; bool requires_provider; bool destructive; } UmiStudioCompilerCommand;
size_t umi_studio_compiler_commands_count(void);
const UmiStudioCompilerCommand *umi_studio_compiler_commands_at(size_t index);
const UmiStudioCompilerCommand *umi_studio_compiler_commands_find(const char *command_id);
#endif
