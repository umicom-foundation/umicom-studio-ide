/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/platform_contract.c
 *
 * PURPOSE:
 *   Implement a semantic Studio composition contract over Framework runtime
 *   inventories.  Required commands and services are checked by identity while
 *   unrelated/optional registry contributions remain free to extend the total.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/platform_contract.h"

#include <string.h>

#include "umicom/studio/commands.h"
#include "umicom/studio/workbench_commands.h"

static const char *const CORE_COMMAND_IDS[] = {
    UMI_STUDIO_COMMAND_SESSION_SAVE,
    UMI_STUDIO_COMMAND_DOCUMENTS_SAVE_ALL,
    UMI_STUDIO_COMMAND_TASKS_WAIT_IDLE,
    UMI_STUDIO_COMMAND_TASKS_REPORT,
    UMI_STUDIO_COMMAND_RECOVERY_PURGE,
    UMI_STUDIO_COMMAND_WORKSPACE_REFRESH,
    UMI_STUDIO_COMMAND_WORKSPACE_CLOSE,
    UMI_STUDIO_COMMAND_WATCHER_SCAN,
    UMI_STUDIO_COMMAND_DATA_INTEGRITY,
    UMI_STUDIO_COMMAND_MESSAGES_FLUSH,
    UMI_STUDIO_COMMAND_MESSAGES_REPLAY,
    UMI_STUDIO_COMMAND_SECURITY_REPORT,
    UMI_STUDIO_COMMAND_PLUGINS_REPORT,
    UMI_STUDIO_COMMAND_MARKETPLACE_CHECK_UPDATES,
    UMI_STUDIO_COMMAND_MARKETPLACE_PLAN_UPDATE,
    UMI_STUDIO_COMMAND_AI_REFRESH_HEALTH,
    UMI_STUDIO_COMMAND_AI_NEW_SESSION,
    UMI_STUDIO_COMMAND_AI_SAVE_SESSION,
    UMI_STUDIO_COMMAND_AI_CODE_CHAT,
    UMI_STUDIO_COMMAND_AI_COMPLETE_CODE,
    UMI_STUDIO_COMMAND_AI_EXPLAIN_CODE,
    UMI_STUDIO_COMMAND_AI_REFACTOR_CODE,
    UMI_STUDIO_COMMAND_AI_GENERATE_TESTS,
    UMI_STUDIO_COMMAND_AI_PATCH_APPROVE,
    UMI_STUDIO_COMMAND_AI_PATCH_APPLY,
    UMI_STUDIO_COMMAND_AI_PATCH_REVERT,
    UMI_STUDIO_COMMAND_KNOWLEDGE_COLLECTION_CREATE,
    UMI_STUDIO_COMMAND_KNOWLEDGE_INGEST,
    UMI_STUDIO_COMMAND_KNOWLEDGE_REFRESH,
    UMI_STUDIO_COMMAND_KNOWLEDGE_SEARCH,
    UMI_STUDIO_COMMAND_KNOWLEDGE_ARCHIVE_SAVE,
    UMI_STUDIO_COMMAND_KNOWLEDGE_ARCHIVE_LOAD,
    UMI_STUDIO_COMMAND_OBSERVABILITY_REPORT,
    UMI_STUDIO_COMMAND_RESILIENCE_REPORT,
    UMI_STUDIO_COMMAND_BUILD_CONFIGURE,
    UMI_STUDIO_COMMAND_BUILD_COMPILE,
    UMI_STUDIO_COMMAND_BUILD_TEST,
    UMI_STUDIO_COMMAND_BUILD_CLEAN,
    UMI_STUDIO_COMMAND_BUILD_RUN,
    UMI_STUDIO_COMMAND_BUILD_INSTALL,
    UMI_STUDIO_COMMAND_BUILD_RETRY,
    UMI_STUDIO_COMMAND_BUILD_CANCEL,
    UMI_STUDIO_COMMAND_BUILD_FILTER,
    UMI_STUDIO_COMMAND_BUILD_SELECT_NODE,
    UMI_STUDIO_COMMAND_BUILD_SELECT_OPERATION,
    UMI_STUDIO_COMMAND_BUILD_SELECT_ARTIFACT,
    UMI_STUDIO_COMMAND_BUILD_RUN_NEXT,
    UMI_STUDIO_COMMAND_BUILD_RUN_ALL,
    UMI_STUDIO_COMMAND_BUILD_INVALIDATE,
    UMI_STUDIO_COMMAND_BUILD_REFRESH,
    UMI_STUDIO_COMMAND_BUILD_CLEAR_HISTORY,
    UMI_STUDIO_COMMAND_TESTS_DISCOVER,
    UMI_STUDIO_COMMAND_TESTS_FILTER,
    UMI_STUDIO_COMMAND_TESTS_SELECT,
    UMI_STUDIO_COMMAND_TESTS_RUN_ALL,
    UMI_STUDIO_COMMAND_TESTS_RUN_SELECTED,
    UMI_STUDIO_COMMAND_TESTS_DEBUG_SELECTED,
    UMI_STUDIO_COMMAND_TESTS_RUN_COVERAGE,
    UMI_STUDIO_COMMAND_TESTS_RERUN_FAILED,
    UMI_STUDIO_COMMAND_TESTS_REPEAT_SELECTED,
    UMI_STUDIO_COMMAND_TESTS_STOP,
    UMI_STUDIO_COMMAND_TESTS_CLEAR_RESULTS,
    UMI_STUDIO_COMMAND_TESTS_CLEAR_OUTPUT,
    UMI_STUDIO_COMMAND_TESTS_CLEAR_COVERAGE,
    UMI_STUDIO_COMMAND_TERMINAL_EXECUTE,
    UMI_STUDIO_COMMAND_TERMINAL_CLEAR,
    UMI_STUDIO_COMMAND_TERMINAL_NEW,
    UMI_STUDIO_COMMAND_TERMINAL_CLOSE,
    UMI_STUDIO_COMMAND_TERMINAL_NEXT,
    UMI_STUDIO_COMMAND_TERMINAL_PREVIOUS,
    UMI_STUDIO_COMMAND_TERMINAL_SPLIT_HORIZONTAL,
    UMI_STUDIO_COMMAND_TERMINAL_SPLIT_VERTICAL,
    UMI_STUDIO_COMMAND_TERMINAL_HISTORY_CLEAR,
    UMI_STUDIO_COMMAND_TERMINAL_SEARCH,
    UMI_STUDIO_COMMAND_PROCESS_REPORT,
    UMI_STUDIO_COMMAND_PROCESS_CANCEL,
    UMI_STUDIO_COMMAND_DIAGNOSTICS_CLEAR,
    UMI_STUDIO_COMMAND_OUTPUT_CLEAR,
    UMI_STUDIO_COMMAND_LANGUAGE_INITIALIZE,
    UMI_STUDIO_COMMAND_LANGUAGE_WORKSPACE_SYMBOLS,
    UMI_STUDIO_COMMAND_DEBUG_INITIALIZE,
    UMI_STUDIO_COMMAND_DEBUG_START,
    UMI_STUDIO_COMMAND_DEBUG_CONTINUE,
    UMI_STUDIO_COMMAND_DEBUG_PAUSE,
    UMI_STUDIO_COMMAND_DEBUG_NEXT,
    UMI_STUDIO_COMMAND_DEBUG_STEP_IN,
    UMI_STUDIO_COMMAND_DEBUG_STEP_OUT,
    UMI_STUDIO_COMMAND_DEBUG_STOP,
    UMI_STUDIO_COMMAND_DEBUG_ADD_BREAKPOINT,
    UMI_STUDIO_COMMAND_DEBUG_SET_BREAKPOINT_ENABLED,
    UMI_STUDIO_COMMAND_DEBUG_REMOVE_BREAKPOINT,
    UMI_STUDIO_COMMAND_DEBUG_ADD_WATCH,
    UMI_STUDIO_COMMAND_DEBUG_REMOVE_WATCH,
    UMI_STUDIO_COMMAND_DEBUG_SELECT_THREAD,
    UMI_STUDIO_COMMAND_DEBUG_SELECT_FRAME,
    UMI_STUDIO_COMMAND_DEBUG_SELECT_SCOPE,
    UMI_STUDIO_COMMAND_DEBUG_CLEAR_CONSOLE,
    UMI_STUDIO_COMMAND_VCS_REFRESH,
    UMI_STUDIO_COMMAND_VCS_STAGE,
    UMI_STUDIO_COMMAND_VCS_UNSTAGE,
    UMI_STUDIO_COMMAND_VCS_STAGE_ALL,
    UMI_STUDIO_COMMAND_VCS_UNSTAGE_ALL,
    UMI_STUDIO_COMMAND_VCS_DISCARD,
    UMI_STUDIO_COMMAND_VCS_COMMIT,
    UMI_STUDIO_COMMAND_VCS_FETCH,
    UMI_STUDIO_COMMAND_VCS_PULL,
    UMI_STUDIO_COMMAND_VCS_PUSH,
    UMI_STUDIO_COMMAND_VCS_BRANCH_CREATE,
    UMI_STUDIO_COMMAND_VCS_BRANCH_CHECKOUT,
    UMI_STUDIO_COMMAND_VCS_BRANCH_DELETE,
    UMI_STUDIO_COMMAND_VCS_DIFF,
    UMI_STUDIO_COMMAND_VCS_FILTER,
    UMI_STUDIO_COMMAND_VCS_SELECT_CHANGE,
    UMI_STUDIO_COMMAND_VCS_SELECT_COMMIT,
    UMI_STUDIO_COMMAND_VCS_SELECT_BRANCH,
    UMI_STUDIO_COMMAND_VCS_SELECT_REMOTE,
    UMI_STUDIO_COMMAND_VCS_SET_COMMIT_MESSAGE,
    UMI_STUDIO_COMMAND_VCS_STAGE_SELECTED,
    UMI_STUDIO_COMMAND_VCS_UNSTAGE_SELECTED,
    UMI_STUDIO_COMMAND_VCS_DISCARD_SELECTED,
    UMI_STUDIO_COMMAND_VCS_COMMIT_COMPOSED,
    UMI_STUDIO_COMMAND_VCS_DIFF_SELECTED,
    UMI_STUDIO_COMMAND_VCS_DIFF_SELECTED_STAGED,
    UMI_STUDIO_COMMAND_TRADING_REFRESH,
    UMI_STUDIO_COMMAND_TRADING_FILTER_INSTRUMENTS,
    UMI_STUDIO_COMMAND_TRADING_SELECT_INSTRUMENT,
    UMI_STUDIO_COMMAND_TRADING_SET_ENVIRONMENT,
    UMI_STUDIO_COMMAND_TRADING_SET_SIDE,
    UMI_STUDIO_COMMAND_TRADING_SET_TYPE,
    UMI_STUDIO_COMMAND_TRADING_SET_QUANTITY,
    UMI_STUDIO_COMMAND_TRADING_SET_PRICES,
    UMI_STUDIO_COMMAND_TRADING_PREVIEW_ORDER,
    UMI_STUDIO_COMMAND_TRADING_SUBMIT_ORDER,
    UMI_STUDIO_COMMAND_TRADING_FILTER_ORDERS,
    UMI_STUDIO_COMMAND_TRADING_SELECT_ORDER,
    UMI_STUDIO_COMMAND_TRADING_CANCEL_ORDER,
    UMI_STUDIO_COMMAND_TRADING_ENGAGE_KILL_SWITCH,
    UMI_STUDIO_COMMAND_TRADING_RESET_KILL_SWITCH,
    UMI_STUDIO_COMMAND_DEVELOPER_REPORT
};

static const char *const WORKBENCH_COMMAND_IDS[] = {
    UMI_STUDIO_COMMAND_PERSPECTIVE_ACTIVATE,
    UMI_STUDIO_COMMAND_ACTIVITY_ACTIVATE,
    UMI_STUDIO_COMMAND_PANE_TOGGLE,
    UMI_STUDIO_COMMAND_SIDEBAR_TOGGLE,
    UMI_STUDIO_COMMAND_BOTTOM_PANEL_TOGGLE,
    UMI_STUDIO_COMMAND_AUXILIARY_TOGGLE,
    UMI_STUDIO_COMMAND_STATUS_SET,
    UMI_STUDIO_COMMAND_LAYOUT_RESET,
    UMI_STUDIO_COMMAND_WORKSPACE_PROFILE_ACTIVATE,
    UMI_STUDIO_COMMAND_NOTIFICATION_INFO,
    UMI_STUDIO_COMMAND_QUICK_ACCESS_SHOW,
    UMI_STUDIO_COMMAND_EDITOR_CLOSE_OTHERS,
    UMI_STUDIO_COMMAND_EDITOR_CLOSE_ALL,
    UMI_STUDIO_COMMAND_EDITOR_PIN_TOGGLE,
    UMI_STUDIO_COMMAND_EDITOR_PREVIEW_PROMOTE,
    UMI_STUDIO_COMMAND_EDITOR_WORD_WRAP_TOGGLE,
    UMI_STUDIO_COMMAND_EDITOR_NEXT,
    UMI_STUDIO_COMMAND_EDITOR_PREVIOUS,
    UMI_STUDIO_COMMAND_EDITOR_SPLIT_RIGHT,
    UMI_STUDIO_COMMAND_EDITOR_SPLIT_DOWN,
    UMI_STUDIO_COMMAND_EDITOR_MOVE_NEXT_GROUP,
    UMI_STUDIO_COMMAND_EDITOR_MOVE_PREVIOUS_GROUP,
    UMI_STUDIO_COMMAND_EDITOR_FOCUS_NEXT_GROUP,
    UMI_STUDIO_COMMAND_EDITOR_BALANCE_GROUPS,
    UMI_STUDIO_COMMAND_EDITOR_RESET_GROUPS
};

/*
 * Studio deliberately binds these reusable Framework designer commands during
 * service publication.  They are required contributions, but they are not
 * counted as Studio core/workbench commands because Framework owns the IDs.
 */
static const char *const CONTRIBUTED_COMMAND_IDS[] = {
    "designer.undo",
    "designer.redo"
};

/*
 * This list deliberately names the stable services that define the minimum
 * Studio product composition.  Additional Framework and Studio services are
 * allowed and are reported through the inventory snapshot.
 */
static const char *const REQUIRED_SERVICE_IDS[] = {
    "umicom.studio.settings",
    "umicom.studio.diagnostics.store",
    "umicom.studio.tasks",
    "umicom.studio.documents",
    "umicom.studio.session",
    "umicom.studio.recovery",
    "umicom.studio.workspace",
    "umicom.studio.file-index",
    "umicom.studio.watcher",
    "umicom.studio.process-supervisor",
    "umicom.studio.data-server",
    "umicom.studio.developer-platform",
    "umicom.studio.build",
    "umicom.studio.tests",
    "umicom.studio.terminal",
    "umicom.studio.terminal-controller",
    "umicom.studio.language",
    "umicom.studio.debugger",
    "umicom.studio.source-control",
    "umicom.studio.vcs-workspace",
    "umicom.studio.trading",
    "umicom.studio.trading-workspace",
    "umicom.studio.designer",
    "umicom.studio.ai-platform",
    "umicom.studio.authorengine-integration",
    "umicom.studio.ai-coding-assistant",
    "umicom.studio.knowledge-centre",
    "umicom.studio.delivery-platform",
    "umicom.studio.product-centre",
    "umicom.studio.distribution-service",
    "umicom.studio.clock"
};

#define ARRAY_COUNT(values) (sizeof(values) / sizeof((values)[0]))

size_t umi_studio_platform_contract_core_command_count(void)
{
    return ARRAY_COUNT(CORE_COMMAND_IDS);
}

const char *umi_studio_platform_contract_core_command_id(size_t index)
{
    return index < ARRAY_COUNT(CORE_COMMAND_IDS) ? CORE_COMMAND_IDS[index] : NULL;
}

size_t umi_studio_platform_contract_workbench_command_count(void)
{
    return ARRAY_COUNT(WORKBENCH_COMMAND_IDS);
}

const char *umi_studio_platform_contract_workbench_command_id(size_t index)
{
    return index < ARRAY_COUNT(WORKBENCH_COMMAND_IDS)
        ? WORKBENCH_COMMAND_IDS[index]
        : NULL;
}

size_t umi_studio_platform_contract_contributed_command_count(void)
{
    return ARRAY_COUNT(CONTRIBUTED_COMMAND_IDS);
}

const char *umi_studio_platform_contract_contributed_command_id(size_t index)
{
    return index < ARRAY_COUNT(CONTRIBUTED_COMMAND_IDS)
        ? CONTRIBUTED_COMMAND_IDS[index]
        : NULL;
}

size_t umi_studio_platform_contract_required_service_count(void)
{
    return ARRAY_COUNT(REQUIRED_SERVICE_IDS);
}

const char *umi_studio_platform_contract_required_service_id(size_t index)
{
    return index < ARRAY_COUNT(REQUIRED_SERVICE_IDS)
        ? REQUIRED_SERVICE_IDS[index]
        : NULL;
}

static size_t count_missing_commands(const UmiCommandRegistry *commands,
                                     const char *const *ids,
                                     size_t count)
{
    size_t index;
    size_t missing = 0U;

    for (index = 0U; index < count; ++index) {
        if (!umi_runtime_inventory_has_command(commands, ids[index])) {
            ++missing;
        }
    }
    return missing;
}

static size_t count_missing_services(const UmiServiceRegistry *services)
{
    size_t index;
    size_t missing = 0U;

    for (index = 0U; index < ARRAY_COUNT(REQUIRED_SERVICE_IDS); ++index) {
        if (!umi_runtime_inventory_has_service(services,
                                               REQUIRED_SERVICE_IDS[index])) {
            ++missing;
        }
    }
    return missing;
}

UmiStatus umi_studio_platform_contract_capture(
    const UmiCommandRegistry *commands,
    const UmiServiceRegistry *services,
    UmiStudioPlatformContractSnapshot *out_snapshot)
{
    UmiStatus status;

    if (commands == NULL || services == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_STUDIO_PLATFORM_CONTRACT_API_VERSION;

    status = umi_runtime_inventory_snapshot(commands,
                                            services,
                                            NULL,
                                            &out_snapshot->runtime);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    out_snapshot->expected_core_command_count = ARRAY_COUNT(CORE_COMMAND_IDS);
    out_snapshot->expected_workbench_command_count =
        ARRAY_COUNT(WORKBENCH_COMMAND_IDS);
    out_snapshot->expected_studio_command_count =
        out_snapshot->expected_core_command_count +
        out_snapshot->expected_workbench_command_count;
    out_snapshot->required_contributed_command_count =
        ARRAY_COUNT(CONTRIBUTED_COMMAND_IDS);
    out_snapshot->expected_minimum_command_count =
        out_snapshot->expected_studio_command_count +
        out_snapshot->required_contributed_command_count;
    out_snapshot->required_service_count = ARRAY_COUNT(REQUIRED_SERVICE_IDS);

    out_snapshot->missing_core_command_count = count_missing_commands(
        commands, CORE_COMMAND_IDS, ARRAY_COUNT(CORE_COMMAND_IDS));
    out_snapshot->missing_workbench_command_count = count_missing_commands(
        commands, WORKBENCH_COMMAND_IDS, ARRAY_COUNT(WORKBENCH_COMMAND_IDS));
    out_snapshot->missing_contributed_command_count = count_missing_commands(
        commands, CONTRIBUTED_COMMAND_IDS, ARRAY_COUNT(CONTRIBUTED_COMMAND_IDS));
    out_snapshot->missing_required_service_count = count_missing_services(services);

    status = umi_runtime_inventory_count_command_prefix(
        commands, "studio.", &out_snapshot->studio_namespace_command_count);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_runtime_inventory_count_service_prefix(
        services, "umicom.studio.", &out_snapshot->studio_namespace_service_count);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    out_snapshot->core_commands_complete =
        out_snapshot->missing_core_command_count == 0U;
    out_snapshot->workbench_commands_complete =
        out_snapshot->missing_workbench_command_count == 0U;
    out_snapshot->contributed_commands_complete =
        out_snapshot->missing_contributed_command_count == 0U;
    out_snapshot->required_services_complete =
        out_snapshot->missing_required_service_count == 0U;
    out_snapshot->valid =
        out_snapshot->core_commands_complete &&
        out_snapshot->workbench_commands_complete &&
        out_snapshot->contributed_commands_complete &&
        out_snapshot->required_services_complete &&
        out_snapshot->runtime.command_count >=
            out_snapshot->expected_minimum_command_count;

    return UMI_STATUS_OK;
}

UmiStatus umi_studio_platform_contract_capture_bootstrap(
    UmiStudioBootstrap *bootstrap,
    UmiStudioPlatformContractSnapshot *out_snapshot)
{
    if (bootstrap == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_studio_platform_contract_capture(
        umi_studio_bootstrap_command_registry(bootstrap),
        umi_studio_bootstrap_service_registry(bootstrap),
        out_snapshot);
}

UmiStatus umi_studio_platform_contract_validate(
    const UmiStudioPlatformContractSnapshot *snapshot)
{
    if (snapshot == NULL ||
        snapshot->structure_size < sizeof(*snapshot) ||
        snapshot->api_version != UMI_STUDIO_PLATFORM_CONTRACT_API_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return snapshot->valid ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
