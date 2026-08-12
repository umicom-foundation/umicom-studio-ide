/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_platform_contract_missing.c
 *
 * PURPOSE:
 *   Verify that a partial registry is reported as an invalid Studio composition
 *   with explicit missing-command and missing-service counts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/platform_contract.h"

#include <assert.h>

int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiServiceRegistry *services = NULL;
    UmiStudioPlatformContractSnapshot snapshot;

    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    assert(umi_service_registry_create(&services) == UMI_STATUS_OK);
    assert(umi_studio_platform_contract_capture(
        commands, services, &snapshot) == UMI_STATUS_OK);
    assert(!snapshot.valid);
    assert(snapshot.missing_core_command_count ==
           snapshot.expected_core_command_count);
    assert(snapshot.missing_workbench_command_count ==
           snapshot.expected_workbench_command_count);
    assert(snapshot.missing_contributed_command_count ==
           snapshot.required_contributed_command_count);
    assert(snapshot.missing_required_service_count ==
           snapshot.required_service_count);
    assert(umi_studio_platform_contract_validate(&snapshot) ==
           UMI_STATUS_INVALID_STATE);

    umi_service_registry_destroy(services);
    umi_command_registry_destroy(commands);
    return 0;
}
