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
#define UMI_STUDIO_COMMAND_AI_REFRESH_HEALTH "studio.ai.refresh-health"
#define UMI_STUDIO_COMMAND_AI_NEW_SESSION "studio.ai.new-session"
#define UMI_STUDIO_COMMAND_AI_SAVE_SESSION "studio.ai.save-session"
#define UMI_STUDIO_COMMAND_AI_CODE_CHAT "studio.ai.code-chat"
#define UMI_STUDIO_COMMAND_AI_COMPLETE_CODE "studio.ai.complete-code"
#define UMI_STUDIO_COMMAND_AI_EXPLAIN_CODE "studio.ai.explain-code"
#define UMI_STUDIO_COMMAND_AI_REFACTOR_CODE "studio.ai.refactor-code"
#define UMI_STUDIO_COMMAND_AI_GENERATE_TESTS "studio.ai.generate-tests"
#define UMI_STUDIO_COMMAND_AI_PATCH_APPROVE "studio.ai.patch-approve"
#define UMI_STUDIO_COMMAND_AI_PATCH_APPLY "studio.ai.patch-apply"
#define UMI_STUDIO_COMMAND_AI_PATCH_REVERT "studio.ai.patch-revert"
#define UMI_STUDIO_COMMAND_KNOWLEDGE_COLLECTION_CREATE \
    "studio.knowledge.collection-create"
#define UMI_STUDIO_COMMAND_KNOWLEDGE_INGEST "studio.knowledge.ingest"
#define UMI_STUDIO_COMMAND_KNOWLEDGE_REFRESH "studio.knowledge.refresh"
#define UMI_STUDIO_COMMAND_KNOWLEDGE_SEARCH "studio.knowledge.search"
#define UMI_STUDIO_COMMAND_KNOWLEDGE_ARCHIVE_SAVE \
    "studio.knowledge.archive-save"
#define UMI_STUDIO_COMMAND_KNOWLEDGE_ARCHIVE_LOAD \
    "studio.knowledge.archive-load"
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
#define UMI_STUDIO_COMMAND_BUILD_FILTER "studio.build.filter"
#define UMI_STUDIO_COMMAND_BUILD_SELECT_NODE "studio.build.select-node"
#define UMI_STUDIO_COMMAND_BUILD_SELECT_OPERATION \
    "studio.build.select-operation"
#define UMI_STUDIO_COMMAND_BUILD_SELECT_ARTIFACT \
    "studio.build.select-artifact"
#define UMI_STUDIO_COMMAND_BUILD_RUN_NEXT "studio.build.run-next"
#define UMI_STUDIO_COMMAND_BUILD_RUN_ALL "studio.build.run-all"
#define UMI_STUDIO_COMMAND_BUILD_INVALIDATE "studio.build.invalidate"
#define UMI_STUDIO_COMMAND_BUILD_REFRESH "studio.build.refresh"
#define UMI_STUDIO_COMMAND_BUILD_CLEAR_HISTORY "studio.build.clear-history"
#define UMI_STUDIO_COMMAND_TESTS_DISCOVER "studio.tests.discover"
#define UMI_STUDIO_COMMAND_TESTS_FILTER "studio.tests.filter"
#define UMI_STUDIO_COMMAND_TESTS_SELECT "studio.tests.select"
#define UMI_STUDIO_COMMAND_TESTS_RUN_ALL "studio.tests.run-all"
#define UMI_STUDIO_COMMAND_TESTS_RUN_SELECTED "studio.tests.run-selected"
#define UMI_STUDIO_COMMAND_TESTS_DEBUG_SELECTED "studio.tests.debug-selected"
#define UMI_STUDIO_COMMAND_TESTS_RUN_COVERAGE "studio.tests.run-coverage"
#define UMI_STUDIO_COMMAND_TESTS_RERUN_FAILED "studio.tests.rerun-failed"
#define UMI_STUDIO_COMMAND_TESTS_REPEAT_SELECTED "studio.tests.repeat-selected"
#define UMI_STUDIO_COMMAND_TESTS_STOP "studio.tests.stop"
#define UMI_STUDIO_COMMAND_TESTS_CLEAR_RESULTS "studio.tests.clear-results"
#define UMI_STUDIO_COMMAND_TESTS_CLEAR_OUTPUT "studio.tests.clear-output"
#define UMI_STUDIO_COMMAND_TESTS_CLEAR_COVERAGE "studio.tests.clear-coverage"
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
#define UMI_STUDIO_COMMAND_DEBUG_SET_BREAKPOINT_ENABLED \
    "studio.debug.set-breakpoint-enabled"
#define UMI_STUDIO_COMMAND_DEBUG_REMOVE_BREAKPOINT \
    "studio.debug.remove-breakpoint"
#define UMI_STUDIO_COMMAND_DEBUG_ADD_WATCH "studio.debug.add-watch"
#define UMI_STUDIO_COMMAND_DEBUG_REMOVE_WATCH "studio.debug.remove-watch"
#define UMI_STUDIO_COMMAND_DEBUG_SELECT_THREAD "studio.debug.select-thread"
#define UMI_STUDIO_COMMAND_DEBUG_SELECT_FRAME "studio.debug.select-frame"
#define UMI_STUDIO_COMMAND_DEBUG_SELECT_SCOPE "studio.debug.select-scope"
#define UMI_STUDIO_COMMAND_DEBUG_CLEAR_CONSOLE "studio.debug.clear-console"
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
#define UMI_STUDIO_COMMAND_VCS_FILTER "studio.vcs.filter"
#define UMI_STUDIO_COMMAND_VCS_SELECT_CHANGE "studio.vcs.select-change"
#define UMI_STUDIO_COMMAND_VCS_SELECT_COMMIT "studio.vcs.select-commit"
#define UMI_STUDIO_COMMAND_VCS_SELECT_BRANCH "studio.vcs.select-branch"
#define UMI_STUDIO_COMMAND_VCS_SELECT_REMOTE "studio.vcs.select-remote"
#define UMI_STUDIO_COMMAND_VCS_SET_COMMIT_MESSAGE \
    "studio.vcs.set-commit-message"
#define UMI_STUDIO_COMMAND_VCS_STAGE_SELECTED "studio.vcs.stage-selected"
#define UMI_STUDIO_COMMAND_VCS_UNSTAGE_SELECTED "studio.vcs.unstage-selected"
#define UMI_STUDIO_COMMAND_VCS_DISCARD_SELECTED "studio.vcs.discard-selected"
#define UMI_STUDIO_COMMAND_VCS_COMMIT_COMPOSED "studio.vcs.commit-composed"
#define UMI_STUDIO_COMMAND_VCS_DIFF_SELECTED "studio.vcs.diff-selected"
#define UMI_STUDIO_COMMAND_VCS_DIFF_SELECTED_STAGED \
    "studio.vcs.diff-selected-staged"
#define UMI_STUDIO_COMMAND_TRADING_REFRESH "studio.trading.refresh"
#define UMI_STUDIO_COMMAND_TRADING_FILTER_INSTRUMENTS \
    "studio.trading.filter-instruments"
#define UMI_STUDIO_COMMAND_TRADING_SELECT_INSTRUMENT \
    "studio.trading.select-instrument"
#define UMI_STUDIO_COMMAND_TRADING_SET_ENVIRONMENT \
    "studio.trading.set-environment"
#define UMI_STUDIO_COMMAND_TRADING_SET_SIDE "studio.trading.set-side"
#define UMI_STUDIO_COMMAND_TRADING_SET_TYPE "studio.trading.set-type"
#define UMI_STUDIO_COMMAND_TRADING_SET_QUANTITY \
    "studio.trading.set-quantity"
#define UMI_STUDIO_COMMAND_TRADING_SET_PRICES "studio.trading.set-prices"
#define UMI_STUDIO_COMMAND_TRADING_PREVIEW_ORDER \
    "studio.trading.preview-order"
#define UMI_STUDIO_COMMAND_TRADING_SUBMIT_ORDER \
    "studio.trading.submit-order"
#define UMI_STUDIO_COMMAND_TRADING_FILTER_ORDERS \
    "studio.trading.filter-orders"
#define UMI_STUDIO_COMMAND_TRADING_SELECT_ORDER \
    "studio.trading.select-order"
#define UMI_STUDIO_COMMAND_TRADING_CANCEL_ORDER \
    "studio.trading.cancel-order"
#define UMI_STUDIO_COMMAND_TRADING_ENGAGE_KILL_SWITCH \
    "studio.trading.engage-kill-switch"
#define UMI_STUDIO_COMMAND_TRADING_RESET_KILL_SWITCH \
    "studio.trading.reset-kill-switch"
#define UMI_STUDIO_COMMAND_DEVELOPER_REPORT "studio.developer.report"

#define UMI_STUDIO_CORE_COMMAND_COUNT 139U

UmiStatus umi_studio_commands_register(UmiCommandRegistry *registry,
                                       UmiStudioServices *services);

#ifdef __cplusplus
}
#endif

#endif
