/* Umicom Studio IDE | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_FRONTEND_STUDIO_COMMANDS_H
#define UMICOM_STUDIO_FRONTEND_STUDIO_COMMANDS_H
#include "umicom/studio/frontend_studio_centre.h"
#define UMI_STUDIO_FRONTEND_COMMAND_COUNT 9U
typedef struct UmiStudioFrontendCommand { const char *id; const char *title; int mutates_state; } UmiStudioFrontendCommand;
size_t umi_studio_frontend_commands_count(void);
const UmiStudioFrontendCommand *umi_studio_frontend_command_at(size_t index);
UmiStatus umi_studio_frontend_command_execute(UmiStudioFrontendCentre *centre,const char *command_id,const char *argument,char *message,size_t message_capacity);
#endif
