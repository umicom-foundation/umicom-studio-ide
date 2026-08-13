/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_state.c
 *
 * PURPOSE:
 *   Verify the immutable Studio operational report assembled from Framework
 *   modules, services, commands, health, tasks, documents and session state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/state.h"
#include "umicom/studio/commands.h"
#include "umicom/studio/platform_contract.h"
#include "umicom/studio/workbench_commands.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioStateReport report;
    UmiStudioPlatformContractSnapshot contract;
    char text[1024];

    (void)umi_fs_remove_tree(".umicom");
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_state_capture(bootstrap, &report) == UMI_STATUS_OK);
    assert(umi_studio_platform_contract_capture_bootstrap(
        bootstrap, &contract) == UMI_STATUS_OK);
    assert(umi_studio_platform_contract_validate(&contract) == UMI_STATUS_OK);

    assert(report.module_count == 1U);
    assert(report.service_count >= 31U);
    /*
     * Retain the previous exact-count regression check for deliberately
     * exclusive-registry builds.  Normal composition allows Framework and
     * designer command contributions in addition to Studio-owned commands.
     */
#if defined(UMI_STUDIO_ENFORCE_EXCLUSIVE_COMMAND_REGISTRY)
    assert(report.command_count ==
           UMI_STUDIO_CORE_COMMAND_COUNT + UMI_STUDIO_WORKBENCH_COMMAND_COUNT);
#endif
    assert(report.command_count == contract.runtime.command_count);
    assert(report.command_count >= contract.expected_minimum_command_count);
    assert(report.health_count >= 3U);
    assert(report.overall_health == UMI_HEALTH_READY);
    assert(report.tasks.worker_count == 4U);
    /* The Framework coordinator imports Studio's editable Welcome.c working
     * copy, so the authoritative store is no longer disconnected from UI. */
    assert(report.document_count >= 1U);
    assert(report.workspace.open);
    assert(report.files.capacity > 0U);
    assert(report.processes.capacity > 0U);
    assert(report.operations.identities == 1U);
    assert(report.operations.supervised_components == 4U);
    assert(report.operations.ready);

    assert(umi_studio_state_format(&report,
                                   text,
                                   sizeof(text)) == UMI_STATUS_OK);
    assert(strstr(text, "Overall health: ready") != NULL);
    assert(strstr(text, "Workspace open: yes") != NULL);
    assert(strstr(text, "Indexed files:") != NULL);

    assert(umi_studio_bootstrap_stop(bootstrap) == UMI_STATUS_OK);
    umi_studio_bootstrap_destroy(bootstrap);
    assert(umi_fs_remove_tree(".umicom") == UMI_STATUS_OK);
    return 0;
}
