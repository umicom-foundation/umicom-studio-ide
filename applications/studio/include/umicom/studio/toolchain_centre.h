/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/toolchain_centre.h
 *
 * PURPOSE:
 *   Compose Framework-owned toolchain discovery, capabilities, SDK metadata,
 *   child environments, compilation databases and per-project compiler choices
 *   into one Studio-facing centre without duplicating reusable mechanisms.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_TOOLCHAIN_CENTRE_H
#define UMICOM_STUDIO_TOOLCHAIN_CENTRE_H

#include <stdint.h>

#include "umicom/toolchain/toolchain.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_TOOLCHAIN_CENTRE_API_VERSION 1U

typedef struct UmiStudioToolchainCentre UmiStudioToolchainCentre;

typedef struct UmiStudioToolchainCentreSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t revision;
    UmiToolchainCatalogueSnapshot catalogue;
    UmiToolchainCapabilitySnapshot selected_capability;
    UmiSdkCatalogueSnapshot selected_sdks;
    UmiCompilationDatabaseSnapshot compilation_database;
    size_t environment_variable_count;
    int has_selected_profile;
    int environment_ready;
    int has_compilation_database;
    int available;
} UmiStudioToolchainCentreSnapshot;

UmiStatus umi_studio_toolchain_centre_create(
    UmiStudioToolchainCentre **out_centre);
void umi_studio_toolchain_centre_destroy(UmiStudioToolchainCentre *centre);
UmiStatus umi_studio_toolchain_centre_discover(
    UmiStudioToolchainCentre *centre,
    const UmiToolchainCatalogueDiscoveryRequest *request,
    UmiToolchainCatalogueDiscoverySnapshot *out_discovery);
UmiStatus umi_studio_toolchain_centre_select_profile(
    UmiStudioToolchainCentre *centre,
    const char *profile_id);
UmiStatus umi_studio_toolchain_centre_bind_project(
    UmiStudioToolchainCentre *centre,
    const UmiToolchainProjectBindingSnapshot *binding);
UmiStatus umi_studio_toolchain_centre_select_project(
    UmiStudioToolchainCentre *centre,
    const char *project_id,
    int require_c23);
UmiStatus umi_studio_toolchain_centre_load_compilation_database(
    UmiStudioToolchainCentre *centre,
    const char *path);
UmiStatus umi_studio_toolchain_centre_discover_compilation_database(
    UmiStudioToolchainCentre *centre,
    const char *project_root,
    const char *const *candidate_build_directories,
    size_t candidate_build_directory_count,
    char *out_path,
    size_t capacity);
UmiStatus umi_studio_toolchain_centre_snapshot(
    UmiStudioToolchainCentre *centre,
    UmiStudioToolchainCentreSnapshot *out_snapshot);
const UmiToolchainProfile *umi_studio_toolchain_centre_selected_profile(
    const UmiStudioToolchainCentre *centre);
const UmiEnvironmentPlan *umi_studio_toolchain_centre_environment(
    const UmiStudioToolchainCentre *centre);
UmiToolchainCatalogue *umi_studio_toolchain_centre_catalogue(
    UmiStudioToolchainCentre *centre);

#ifdef __cplusplus
}
#endif
#endif
