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
