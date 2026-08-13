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
#define UMI_STUDIO_COMMAND_TASKS_REPORT "studio.tasks.report"
#define UMI_STUDIO_COMMAND_RECOVERY_PURGE "studio.recovery.purge"
#define UMI_STUDIO_COMMAND_WORKSPACE_REFRESH "studio.workspace.refresh"
#define UMI_STUDIO_COMMAND_WORKSPACE_CLOSE "studio.workspace.close"
#define UMI_STUDIO_COMMAND_WATCHER_SCAN "studio.watcher.scan"
#define UMI_STUDIO_COMMAND_DATA_INTEGRITY "studio.data.integrity"
#define UMI_STUDIO_COMMAND_MESSAGES_FLUSH "studio.messages.flush"
#define UMI_STUDIO_COMMAND_MESSAGES_REPLAY "studio.messages.replay"
#define UMI_STUDIO_COMMAND_SECURITY_REPORT "studio.security.report"
#define UMI_STUDIO_COMMAND_PLUGINS_REPORT "studio.plugins.report"
#define UMI_STUDIO_COMMAND_MARKETPLACE_CHECK_UPDATES "studio.marketplace.check-updates"
#define UMI_STUDIO_COMMAND_MARKETPLACE_PLAN_UPDATE "studio.marketplace.plan-update"
#define UMI_STUDIO_COMMAND_OBSERVABILITY_REPORT "studio.observability.report"
#define UMI_STUDIO_COMMAND_RESILIENCE_REPORT "studio.resilience.report"
#define UMI_STUDIO_COMMAND_BUILD_CONFIGURE "studio.build.configure"
#define UMI_STUDIO_COMMAND_BUILD_COMPILE "studio.build.compile"
#define UMI_STUDIO_COMMAND_BUILD_TEST "studio.build.test"
#define UMI_STUDIO_COMMAND_BUILD_CLEAN "studio.build.clean"
#define UMI_STUDIO_COMMAND_BUILD_RUN "studio.build.run"
#define UMI_STUDIO_COMMAND_BUILD_INSTALL "studio.build.install"
#define UMI_STUDIO_COMMAND_BUILD_RETRY "studio.build.retry"
#define UMI_STUDIO_COMMAND_BUILD_CANCEL "studio.build.cancel"
#define UMI_STUDIO_COMMAND_TESTS_DISCOVER "studio.tests.discover"
#define UMI_STUDIO_COMMAND_TERMINAL_EXECUTE "studio.terminal.execute"
#define UMI_STUDIO_COMMAND_TERMINAL_CLEAR "studio.terminal.clear"
#define UMI_STUDIO_COMMAND_TERMINAL_NEW "studio.terminal.new"
#define UMI_STUDIO_COMMAND_TERMINAL_CLOSE "studio.terminal.close"
#define UMI_STUDIO_COMMAND_TERMINAL_NEXT "studio.terminal.next"
#define UMI_STUDIO_COMMAND_TERMINAL_PREVIOUS "studio.terminal.previous"
#define UMI_STUDIO_COMMAND_TERMINAL_SPLIT_HORIZONTAL "studio.terminal.split-horizontal"
#define UMI_STUDIO_COMMAND_TERMINAL_SPLIT_VERTICAL "studio.terminal.split-vertical"
#define UMI_STUDIO_COMMAND_TERMINAL_HISTORY_CLEAR "studio.terminal.history-clear"
#define UMI_STUDIO_COMMAND_TERMINAL_SEARCH "studio.terminal.search"
#define UMI_STUDIO_COMMAND_PROCESS_REPORT "studio.process.report"
#define UMI_STUDIO_COMMAND_PROCESS_CANCEL "studio.process.cancel"
#define UMI_STUDIO_COMMAND_DIAGNOSTICS_CLEAR "studio.diagnostics.clear"
#define UMI_STUDIO_COMMAND_OUTPUT_CLEAR "studio.output.clear"
#define UMI_STUDIO_COMMAND_LANGUAGE_INITIALIZE "studio.language.initialize"
#define UMI_STUDIO_COMMAND_LANGUAGE_WORKSPACE_SYMBOLS "studio.language.workspace-symbols"
#define UMI_STUDIO_COMMAND_DEBUG_INITIALIZE "studio.debug.initialize"
#define UMI_STUDIO_COMMAND_DEBUG_START "studio.debug.start"
#define UMI_STUDIO_COMMAND_DEBUG_CONTINUE "studio.debug.continue"
#define UMI_STUDIO_COMMAND_DEBUG_PAUSE "studio.debug.pause"
#define UMI_STUDIO_COMMAND_DEBUG_NEXT "studio.debug.next"
#define UMI_STUDIO_COMMAND_DEBUG_STEP_IN "studio.debug.step-in"
#define UMI_STUDIO_COMMAND_DEBUG_STEP_OUT "studio.debug.step-out"
#define UMI_STUDIO_COMMAND_DEBUG_STOP "studio.debug.stop"
#define UMI_STUDIO_COMMAND_DEBUG_ADD_BREAKPOINT "studio.debug.add-breakpoint"
#define UMI_STUDIO_COMMAND_VCS_REFRESH "studio.vcs.refresh"
#define UMI_STUDIO_COMMAND_VCS_STAGE "studio.vcs.stage"
#define UMI_STUDIO_COMMAND_VCS_UNSTAGE "studio.vcs.unstage"
#define UMI_STUDIO_COMMAND_VCS_STAGE_ALL "studio.vcs.stage-all"
#define UMI_STUDIO_COMMAND_VCS_UNSTAGE_ALL "studio.vcs.unstage-all"
#define UMI_STUDIO_COMMAND_VCS_DISCARD "studio.vcs.discard"
#define UMI_STUDIO_COMMAND_VCS_COMMIT "studio.vcs.commit"
#define UMI_STUDIO_COMMAND_VCS_FETCH "studio.vcs.fetch"
#define UMI_STUDIO_COMMAND_VCS_PULL "studio.vcs.pull"
#define UMI_STUDIO_COMMAND_VCS_PUSH "studio.vcs.push"
#define UMI_STUDIO_COMMAND_VCS_BRANCH_CREATE "studio.vcs.branch-create"
#define UMI_STUDIO_COMMAND_VCS_BRANCH_CHECKOUT "studio.vcs.branch-checkout"
#define UMI_STUDIO_COMMAND_VCS_BRANCH_DELETE "studio.vcs.branch-delete"
#define UMI_STUDIO_COMMAND_VCS_DIFF "studio.vcs.diff"
#define UMI_STUDIO_COMMAND_DEVELOPER_REPORT "studio.developer.report"

#define UMI_STUDIO_CORE_COMMAND_COUNT 66U

UmiStatus umi_studio_commands_register(UmiCommandRegistry *registry,
                                       UmiStudioServices *services);

#ifdef __cplusplus
}
#endif

#endif
