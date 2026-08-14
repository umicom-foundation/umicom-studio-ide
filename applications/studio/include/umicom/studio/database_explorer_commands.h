/* Umicom Studio IDE | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_DATABASE_EXPLORER_COMMANDS_H
#define UMICOM_STUDIO_DATABASE_EXPLORER_COMMANDS_H
#include "umicom/studio/database_explorer_queries.h"
#define UMI_STUDIO_DATABASE_COMMAND_COUNT 10U
typedef struct UmiStudioDatabaseCommand { const char *id; const char *title; int mutates_state; } UmiStudioDatabaseCommand;
size_t umi_studio_database_commands_count(void);
const UmiStudioDatabaseCommand *umi_studio_database_command_at(size_t index);
UmiStatus umi_studio_database_command_execute(UmiStudioDatabaseExplorerCentre *centre,const char *command_id,const char *argument,char *message,size_t message_capacity);
#endif
