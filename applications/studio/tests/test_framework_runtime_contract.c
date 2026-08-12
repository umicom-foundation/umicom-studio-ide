/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_framework_runtime_contract.c
 *
 * PURPOSE:
 *   Verify Studio can consume Framework inventory and compatibility contracts
 *   through the installed aggregate C API boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>

int main(void)
{
    UmiRuntimeContractSnapshot contract;
    UmiRuntimeInventorySnapshot inventory;

    assert(umi_runtime_contract_snapshot(&contract) == UMI_STATUS_OK);
    assert(umi_runtime_inventory_snapshot(NULL, NULL, NULL, &inventory) ==
           UMI_STATUS_OK);
    assert(contract.inventory_snapshot_size == sizeof(inventory));
    assert(contract.framework_abi_version == UMICOM_FRAMEWORK_ABI_VERSION);
    return 0;
}
