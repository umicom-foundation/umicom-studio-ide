/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/knowledge_commands.h
 * PURPOSE: Register Knowledge Centre commands with the shared Studio registry.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: One registry makes these commands available to every frontend. */
#ifndef UMICOM_STUDIO_KNOWLEDGE_COMMANDS_H
#define UMICOM_STUDIO_KNOWLEDGE_COMMANDS_H

#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_knowledge_commands_register(
    UmiCommandRegistry *registry,
    UmiStudioServices *services);

#ifdef __cplusplus
}
#endif
#endif
