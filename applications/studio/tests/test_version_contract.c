/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_version_contract.c
 *
 * PURPOSE:
 *   Verify compile-time Studio version metadata, runtime Studio version data
 *   and Framework ABI contract metadata remain internally consistent.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/contract.h"
#include "umicom/studio/version.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiVersion studio = umi_studio_version();
    UmiRuntimeContractSnapshot framework;

    assert(studio.major == UMICOM_STUDIO_VERSION_MAJOR);
    assert(studio.minor == UMICOM_STUDIO_VERSION_MINOR);
    assert(studio.patch == UMICOM_STUDIO_VERSION_PATCH);
    assert(strcmp(umi_studio_version_string(), UMICOM_STUDIO_VERSION_STRING) == 0);
    assert(umi_runtime_contract_snapshot(&framework) == UMI_STATUS_OK);
    assert(framework.framework_abi_version == UMICOM_FRAMEWORK_ABI_VERSION);
    assert(framework.api_version == UMI_RUNTIME_CONTRACT_API_VERSION);
    return 0;
}
