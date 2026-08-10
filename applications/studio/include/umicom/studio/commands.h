/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/commands.h
 *
 * PURPOSE:
 *   Declare the canonical Studio command identifiers and register product use
 *   cases with the Framework command registry for every frontend and tool.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_COMMANDS_H
#define UMICOM_STUDIO_COMMANDS_H

#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_COMMAND_SESSION_SAVE "studio.session.save"
#define UMI_STUDIO_COMMAND_DOCUMENTS_SAVE_ALL "studio.documents.save-all"
#define UMI_STUDIO_COMMAND_TASKS_WAIT_IDLE "studio.tasks.wait-idle"
#define UMI_STUDIO_COMMAND_RECOVERY_PURGE "studio.recovery.purge"
#define UMI_STUDIO_COMMAND_WORKSPACE_REFRESH "studio.workspace.refresh"
#define UMI_STUDIO_COMMAND_WORKSPACE_CLOSE "studio.workspace.close"
#define UMI_STUDIO_COMMAND_WATCHER_SCAN "studio.watcher.scan"
#define UMI_STUDIO_COMMAND_DATA_INTEGRITY "studio.data.integrity"
#define UMI_STUDIO_COMMAND_MESSAGES_FLUSH "studio.messages.flush"
#define UMI_STUDIO_COMMAND_MESSAGES_REPLAY "studio.messages.replay"
#define UMI_STUDIO_COMMAND_SECURITY_REPORT "studio.security.report"
#define UMI_STUDIO_COMMAND_PLUGINS_REPORT "studio.plugins.report"
#define UMI_STUDIO_COMMAND_OBSERVABILITY_REPORT "studio.observability.report"
#define UMI_STUDIO_COMMAND_RESILIENCE_REPORT "studio.resilience.report"
#define UMI_STUDIO_COMMAND_BUILD_CONFIGURE "studio.build.configure"
#define UMI_STUDIO_COMMAND_BUILD_COMPILE "studio.build.compile"
#define UMI_STUDIO_COMMAND_BUILD_TEST "studio.build.test"
#define UMI_STUDIO_COMMAND_BUILD_CLEAN "studio.build.clean"
#define UMI_STUDIO_COMMAND_TESTS_DISCOVER "studio.tests.discover"
#define UMI_STUDIO_COMMAND_TERMINAL_EXECUTE "studio.terminal.execute"
#define UMI_STUDIO_COMMAND_LANGUAGE_INITIALIZE "studio.language.initialize"
#define UMI_STUDIO_COMMAND_DEBUG_INITIALIZE "studio.debug.initialize"
#define UMI_STUDIO_COMMAND_VCS_REFRESH "studio.vcs.refresh"
#define UMI_STUDIO_COMMAND_DEVELOPER_REPORT "studio.developer.report"

UmiStatus umi_studio_commands_register(UmiCommandRegistry *registry,
                                       UmiStudioServices *services);

#ifdef __cplusplus
}
#endif

#endif
