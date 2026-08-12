/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_platform_contract_api_bounds.c
 *
 * PURPOSE:
 *   Verify safe enumeration bounds and versioned snapshot validation for the
 *   public Studio platform contract API.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/platform_contract.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiStudioPlatformContractSnapshot snapshot;

    assert(umi_studio_platform_contract_core_command_id(
        umi_studio_platform_contract_core_command_count()) == NULL);
    assert(umi_studio_platform_contract_workbench_command_id(
        umi_studio_platform_contract_workbench_command_count()) == NULL);
    assert(umi_studio_platform_contract_contributed_command_id(
        umi_studio_platform_contract_contributed_command_count()) == NULL);
    assert(umi_studio_platform_contract_required_service_id(
        umi_studio_platform_contract_required_service_count()) == NULL);

    (void)memset(&snapshot, 0, sizeof(snapshot));
    assert(umi_studio_platform_contract_validate(&snapshot) ==
           UMI_STATUS_INVALID_ARGUMENT);
    snapshot.structure_size = (uint32_t)sizeof(snapshot);
    snapshot.api_version = UMI_STUDIO_PLATFORM_CONTRACT_API_VERSION;
    assert(umi_studio_platform_contract_validate(&snapshot) ==
           UMI_STATUS_INVALID_STATE);
    return 0;
}
