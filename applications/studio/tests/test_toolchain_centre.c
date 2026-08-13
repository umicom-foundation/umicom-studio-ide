/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_toolchain_centre.c
 *
 * PURPOSE:
 *   Verify that Studio composes a Framework-owned compiler profile into one
 *   coherent capability, SDK and child-environment snapshot.  The fixture is
 *   deliberately deterministic: it exercises centre behaviour without making
 *   the regression test depend on tools installed on the test host.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "umicom/studio/toolchain_centre.h"

static void validate_tool(UmiToolchainProfile *profile,
                          UmiToolKind kind,
                          const char *path,
                          const char *version)
{
    UmiToolInfo *tool = umi_toolchain_profile_tool_mutable(profile, kind);
    tool->state = UMI_TOOL_VALIDATED;
    (void)snprintf(tool->path, sizeof(tool->path), "%s", path);
    (void)snprintf(tool->version, sizeof(tool->version), "%s", version);
}

static void make_profile(UmiToolchainProfile *profile)
{
    umi_toolchain_profile_init(profile);
    (void)strcpy(profile->profile_id, "fixture-gcc");
    (void)strcpy(profile->display_name, "Fixture GCC C23");
    (void)strcpy(profile->root, "/usr");
    (void)strcpy(profile->bin_directory, "/usr/bin");
    (void)strcpy(profile->prefix_directory, "/usr");
    profile->family = UMI_TOOLCHAIN_POSIX_GCC;
    profile->selected_c_compiler = UMI_TOOL_GCC;
    profile->selected_cpp_compiler = UMI_TOOL_GXX;
    profile->c23_capable = 1;
    profile->complete = 1;
    validate_tool(profile, UMI_TOOL_GCC, "/usr/bin/gcc", "gcc 16.1.0");
    validate_tool(profile, UMI_TOOL_GXX, "/usr/bin/g++", "g++ 16.1.0");
    validate_tool(profile, UMI_TOOL_CMAKE, "/usr/bin/cmake", "cmake 4.0.0");
    validate_tool(profile, UMI_TOOL_CTEST, "/usr/bin/ctest", "ctest 4.0.0");
    validate_tool(profile, UMI_TOOL_NINJA, "/usr/bin/ninja", "ninja 1.12.1");
    validate_tool(profile, UMI_TOOL_PKG_CONFIG, "/usr/bin/pkg-config",
                  "pkg-config 2.4.3");
}

int main(void)
{
    UmiStudioToolchainCentre *centre = NULL;
    UmiToolchainProfile profile;
    UmiStudioToolchainCentreSnapshot snapshot;
    const UmiEnvironmentPlan *environment;

    make_profile(&profile);
    if (umi_studio_toolchain_centre_create(&centre) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    if (umi_toolchain_catalogue_upsert_profile(
            umi_studio_toolchain_centre_catalogue(centre), &profile) !=
            UMI_STATUS_OK ||
        umi_studio_toolchain_centre_select_profile(centre, "fixture-gcc") !=
            UMI_STATUS_OK ||
        umi_studio_toolchain_centre_snapshot(centre, &snapshot) !=
            UMI_STATUS_OK) {
        umi_studio_toolchain_centre_destroy(centre);
        return EXIT_FAILURE;
    }
    environment = umi_studio_toolchain_centre_environment(centre);
    if (!snapshot.available || !snapshot.has_selected_profile ||
        !snapshot.environment_ready ||
        snapshot.selected_capability.compiler_vendor !=
            UMI_COMPILER_VENDOR_GCC ||
        snapshot.selected_capability.compiler_version_major != 16U ||
        !snapshot.selected_capability.c23_available ||
        snapshot.selected_sdks.count != 1U || environment == NULL ||
        strcmp(umi_environment_plan_find(environment, "CC"),
               "/usr/bin/gcc") != 0) {
        umi_studio_toolchain_centre_destroy(centre);
        return EXIT_FAILURE;
    }
    umi_studio_toolchain_centre_destroy(centre);
    return EXIT_SUCCESS;
}
