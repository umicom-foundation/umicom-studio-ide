/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_commands.c
 *
 * PURPOSE:
 *   Verify that canonical Studio commands are registered once and execute
 *   through the Framework command registry without a graphical frontend.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/commands.h"
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/platform_contract.h"
#include "umicom/studio/workbench_commands.h"
#include "umicom/studio/tests.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiCommandRegistry *registry;
    UmiCommandSnapshot snapshot;
    UmiStudioPlatformContractSnapshot contract;
    char message[512];

    (void)umi_fs_remove_tree(".umicom");
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);

    registry = umi_studio_bootstrap_command_registry(bootstrap);
    assert(registry != NULL);
    /*
     * The old assertion is retained for builds that intentionally enforce an
     * exclusive Studio-only command registry.  Normal Studio composition also
     * receives reusable designer commands, so the default contract validates
     * required identities while allowing additive registry contributions.
     */
#if defined(UMI_STUDIO_ENFORCE_EXCLUSIVE_COMMAND_REGISTRY)
    assert(umi_command_registry_count(registry) ==
           UMI_STUDIO_CORE_COMMAND_COUNT + UMI_STUDIO_WORKBENCH_COMMAND_COUNT);
#endif
    assert(umi_studio_platform_contract_capture_bootstrap(
        bootstrap, &contract) == UMI_STATUS_OK);
    assert(umi_studio_platform_contract_validate(&contract) == UMI_STATUS_OK);
    assert(umi_command_registry_count(registry) == contract.runtime.command_count);
    assert(umi_command_registry_snapshot(
        registry,
        UMI_STUDIO_COMMAND_SESSION_SAVE,
        &snapshot
    ) == UMI_STATUS_OK);
    assert(strcmp(snapshot.category, "Session") == 0);

    assert(umi_command_registry_execute(
        registry,
        UMI_STUDIO_COMMAND_TASKS_WAIT_IDLE,
        "1000",
        message,
        sizeof(message)
    ) == UMI_STATUS_OK);
    assert(strstr(message, "idle") != NULL);

    assert(umi_command_registry_execute(
        registry,
        UMI_STUDIO_COMMAND_WORKSPACE_REFRESH,
        "",
        message,
        sizeof(message)
    ) == UMI_STATUS_OK);
    assert(strstr(message, "refreshed") != NULL);

    assert(umi_command_registry_execute(
        registry,
        UMI_STUDIO_COMMAND_WATCHER_SCAN,
        "",
        message,
        sizeof(message)
    ) == UMI_STATUS_OK);
    assert(strstr(message, "scanned") != NULL);

    assert(umi_command_registry_execute(
        registry,
        UMI_STUDIO_COMMAND_DATA_INTEGRITY,
        "",
        message,
        sizeof(message)
    ) == UMI_STATUS_OK);
    assert(strstr(message, "integrity OK") != NULL);

    assert(umi_command_registry_execute(
        registry,
        UMI_STUDIO_COMMAND_MESSAGES_FLUSH,
        "",
        message,
        sizeof(message)
    ) == UMI_STATUS_OK);
    assert(strstr(message, "Outbox delivered") != NULL);

    assert(umi_command_registry_execute(
        registry,
        UMI_STUDIO_COMMAND_MESSAGES_REPLAY,
        "",
        message,
        sizeof(message)
    ) == UMI_STATUS_OK);
    assert(strstr(message, "Replayed") != NULL);


    assert(umi_command_registry_execute(
        registry,
        UMI_STUDIO_COMMAND_NOTIFICATION_INFO,
        "Command test notification",
        message,
        sizeof(message)
    ) == UMI_STATUS_OK);
    assert(strstr(message, "published") != NULL);

    assert(umi_command_registry_execute(
        registry,
        UMI_STUDIO_COMMAND_DEVELOPER_REPORT,
        "",
        message,
        sizeof(message)
    ) == UMI_STATUS_OK);
    assert(strstr(message, "Build profile:") != NULL);

    /*
     * Filtering, stable selection and refresh remain entirely inside the
     * Framework coordinator, so this exercises the professional workspace
     * commands without launching a compiler process during the unit test.
     */
    assert(umi_command_registry_snapshot(
               registry, UMI_STUDIO_COMMAND_BUILD_FILTER, &snapshot) ==
           UMI_STATUS_OK);
    assert(strcmp(snapshot.category, "Build") == 0);
    assert(umi_command_registry_execute(
               registry, UMI_STUDIO_COMMAND_BUILD_FILTER, "all",
               message, sizeof(message)) == UMI_STATUS_OK);
    assert(strstr(message, "build node") != NULL);
    assert(umi_command_registry_execute(
               registry, UMI_STUDIO_COMMAND_BUILD_SELECT_NODE, "configure",
               message, sizeof(message)) == UMI_STATUS_OK);
    assert(strstr(message, "configure") != NULL);
    assert(umi_command_registry_execute(
               registry, UMI_STUDIO_COMMAND_BUILD_REFRESH, "",
               message, sizeof(message)) == UMI_STATUS_OK);
    assert(strstr(message, "refreshed") != NULL);

    assert(umi_command_registry_execute(
        registry,
        UMI_STUDIO_COMMAND_AI_EXPLAIN_CODE,
        "applications/studio/src/app/ai_platform.c|Explain ownership.",
        message,
        sizeof(message)
    ) == UMI_STATUS_OK);
    assert(strstr(message, "explanation plan") != NULL);
    assert(strstr(message, "repository file(s)") != NULL);

    assert(umi_command_registry_execute(
        registry,
        UMI_STUDIO_COMMAND_LANGUAGE_INITIALIZE,
        "0",
        message,
        sizeof(message)
    ) == UMI_STATUS_OK);
    assert(strstr(message, "LSP initialise request") != NULL);

    assert(umi_command_registry_execute(
        registry,
        UMI_STUDIO_COMMAND_DEBUG_INITIALIZE,
        "umicom.test.debug",
        message,
        sizeof(message)
    ) == UMI_STATUS_OK);
    assert(strstr(message, "DAP initialise request") != NULL);

    assert(umi_command_registry_execute(
        registry,
        UMI_STUDIO_COMMAND_DEBUG_ADD_BREAKPOINT,
        "main.c:42",
        message,
        sizeof(message)
    ) == UMI_STATUS_OK);
    assert(umi_command_registry_execute(
        registry,
        UMI_STUDIO_COMMAND_DEBUG_SET_BREAKPOINT_ENABLED,
        "main.c:42:1=0",
        message,
        sizeof(message)
    ) == UMI_STATUS_OK);
    assert(umi_command_registry_execute(
        registry,
        UMI_STUDIO_COMMAND_DEBUG_ADD_WATCH,
        "counter + 1",
        message,
        sizeof(message)
    ) == UMI_STATUS_OK);
    assert(strstr(message, "watch-1") != NULL);
    assert(umi_command_registry_execute(
        registry,
        UMI_STUDIO_COMMAND_DEBUG_REMOVE_WATCH,
        "watch-1",
        message,
        sizeof(message)
    ) == UMI_STATUS_OK);
    assert(umi_command_registry_execute(
        registry,
        UMI_STUDIO_COMMAND_DEBUG_CLEAR_CONSOLE,
        "",
        message,
        sizeof(message)
    ) == UMI_STATUS_OK);

    assert(umi_command_registry_snapshot(
               registry, UMI_STUDIO_COMMAND_VCS_FILTER, &snapshot) ==
           UMI_STATUS_OK);
    assert(strcmp(snapshot.category, "Source Control") == 0);
    assert(umi_command_registry_execute(
               registry, UMI_STUDIO_COMMAND_VCS_FILTER, "conflicts",
               message, sizeof(message)) == UMI_STATUS_OK);
    assert(strstr(message, "conflicts") != NULL);
    assert(umi_command_registry_execute(
               registry, UMI_STUDIO_COMMAND_VCS_SET_COMMIT_MESSAGE,
               "feat: professional source control", message,
               sizeof(message)) == UMI_STATUS_OK);
    assert(umi_command_registry_snapshot(
               registry, UMI_STUDIO_COMMAND_VCS_COMMIT_COMPOSED, &snapshot) ==
           UMI_STATUS_OK);

    assert(umi_command_registry_snapshot(
               registry, UMI_STUDIO_COMMAND_TRADING_REFRESH, &snapshot) ==
           UMI_STATUS_OK);
    assert(strcmp(snapshot.category, "Trading") == 0);
    assert(umi_command_registry_execute(
               registry, UMI_STUDIO_COMMAND_TRADING_FILTER_INSTRUMENTS,
               "CME", message, sizeof(message)) == UMI_STATUS_OK);
    assert(umi_command_registry_execute(
               registry, UMI_STUDIO_COMMAND_TRADING_SELECT_INSTRUMENT,
               "CME.ES.REFERENCE", message, sizeof(message)) == UMI_STATUS_OK);
    assert(umi_command_registry_execute(
               registry, UMI_STUDIO_COMMAND_TRADING_PREVIEW_ORDER,
               "", message, sizeof(message)) == UMI_STATUS_OK);
    assert(strstr(message, "allowed") != NULL);

    {
        UmiTestPlatformItemSnapshot item = {0};
        UmiStudioTestService *tests = umi_studio_services_tests(
            umi_studio_bootstrap_services(bootstrap));
        (void)strcpy(item.id, "studio.command.alpha");
        (void)strcpy(item.name, "Studio Command Alpha");
        (void)strcpy(item.kind, "test");
        item.enabled = 1;
        assert(umi_test_platform_item_registry_upsert(
                   umi_test_platform_service_item(
                       umi_studio_test_service_platform(tests)), &item) ==
               UMI_STATUS_OK);
        assert(umi_command_registry_execute(
                   registry, UMI_STUDIO_COMMAND_TESTS_FILTER, "all",
                   message, sizeof(message)) == UMI_STATUS_OK);
        assert(strstr(message, "1 test(s)") != NULL);
        assert(umi_command_registry_execute(
                   registry, UMI_STUDIO_COMMAND_TESTS_SELECT,
                   "studio.command.alpha", message, sizeof(message)) ==
               UMI_STATUS_OK);
        assert(umi_command_registry_snapshot(
                   registry, UMI_STUDIO_COMMAND_TESTS_RUN_SELECTED,
                   &snapshot) == UMI_STATUS_OK);
        assert(strcmp(snapshot.category, "Testing") == 0);
    }

    assert(umi_command_registry_execute(
        registry,
        UMI_STUDIO_COMMAND_SECURITY_REPORT,
        "",
        message,
        sizeof(message)
    ) == UMI_STATUS_OK);
    assert(strstr(message, "Security:") != NULL);

    assert(umi_command_registry_execute(
        registry,
        UMI_STUDIO_COMMAND_SESSION_SAVE,
        "",
        message,
        sizeof(message)
    ) == UMI_STATUS_OK);
    assert(umi_fs_is_file(".umicom/studio.session"));

    assert(umi_studio_bootstrap_stop(bootstrap) == UMI_STATUS_OK);
    umi_studio_bootstrap_destroy(bootstrap);
    assert(umi_fs_remove_tree(".umicom") == UMI_STATUS_OK);
    return 0;
}
