/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/platform_contract.h
 *
 * PURPOSE:
 *   Define the Studio-owned runtime composition contract separately from the
 *   total Framework registry population.  This prevents optional Framework or
 *   designer commands from breaking tests that should validate required Studio
 *   behaviour rather than a fragile magic total.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_PLATFORM_CONTRACT_H
#define UMICOM_STUDIO_PLATFORM_CONTRACT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/runtime/inventory.h"
#include "umicom/studio/bootstrap.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_PLATFORM_CONTRACT_API_VERSION 1U

typedef struct UmiStudioPlatformContractSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    UmiRuntimeInventorySnapshot runtime;
    size_t expected_core_command_count;
    size_t expected_workbench_command_count;
    size_t expected_studio_command_count;
    size_t required_contributed_command_count;
    size_t expected_minimum_command_count;
    size_t missing_core_command_count;
    size_t missing_workbench_command_count;
    size_t missing_contributed_command_count;
    size_t required_service_count;
    size_t missing_required_service_count;
    size_t studio_namespace_command_count;
    size_t studio_namespace_service_count;
    int core_commands_complete;
    int workbench_commands_complete;
    int contributed_commands_complete;
    int required_services_complete;
    int valid;
} UmiStudioPlatformContractSnapshot;

size_t umi_studio_platform_contract_core_command_count(void);
const char *umi_studio_platform_contract_core_command_id(size_t index);
size_t umi_studio_platform_contract_workbench_command_count(void);
const char *umi_studio_platform_contract_workbench_command_id(size_t index);
size_t umi_studio_platform_contract_contributed_command_count(void);
const char *umi_studio_platform_contract_contributed_command_id(size_t index);
size_t umi_studio_platform_contract_required_service_count(void);
const char *umi_studio_platform_contract_required_service_id(size_t index);

UmiStatus umi_studio_platform_contract_capture(
    const UmiCommandRegistry *commands,
    const UmiServiceRegistry *services,
    UmiStudioPlatformContractSnapshot *out_snapshot);
UmiStatus umi_studio_platform_contract_capture_bootstrap(
    UmiStudioBootstrap *bootstrap,
    UmiStudioPlatformContractSnapshot *out_snapshot);
UmiStatus umi_studio_platform_contract_validate(
    const UmiStudioPlatformContractSnapshot *snapshot);

#ifdef __cplusplus
}
#endif

#endif
