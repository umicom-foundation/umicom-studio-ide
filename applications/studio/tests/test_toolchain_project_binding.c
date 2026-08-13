/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_toolchain_project_binding.c
 * PURPOSE: Verify that separate projects can select separate C23 profiles.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "umicom/studio/toolchain_centre.h"

static void make_profile(UmiToolchainProfile *profile,
                         const char *id,
                         UmiToolKind compiler)
{
    UmiToolInfo *tool;
    umi_toolchain_profile_init(profile);
    (void)snprintf(profile->profile_id, sizeof(profile->profile_id), "%s", id);
    (void)strcpy(profile->bin_directory, "/fixture/bin");
    profile->selected_c_compiler = compiler;
    profile->selected_cpp_compiler = compiler == UMI_TOOL_GCC
        ? UMI_TOOL_GXX : UMI_TOOL_CLANGXX;
    profile->complete = 1;
    profile->c23_capable = 1;
    tool = umi_toolchain_profile_tool_mutable(profile, compiler);
    tool->state = UMI_TOOL_VALIDATED;
    (void)snprintf(tool->path, sizeof(tool->path), "/fixture/bin/%s",
                   compiler == UMI_TOOL_GCC ? "gcc" : "clang");
    umi_toolchain_profile_tool_mutable(profile, UMI_TOOL_CMAKE)->state =
        UMI_TOOL_VALIDATED;
    umi_toolchain_profile_tool_mutable(profile, UMI_TOOL_NINJA)->state =
        UMI_TOOL_VALIDATED;
}

int main(void)
{
    UmiStudioToolchainCentre *centre = NULL;
    UmiToolchainProfile gcc_profile;
    UmiToolchainProfile clang_profile;
    UmiToolchainProjectBindingSnapshot binding = {0};
    const UmiToolchainProfile *selected;

    make_profile(&gcc_profile, "fixture-gcc", UMI_TOOL_GCC);
    make_profile(&clang_profile, "fixture-clang", UMI_TOOL_CLANG);
    if (umi_studio_toolchain_centre_create(&centre) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    if (umi_toolchain_catalogue_upsert_profile(
            umi_studio_toolchain_centre_catalogue(centre), &gcc_profile) !=
            UMI_STATUS_OK ||
        umi_toolchain_catalogue_upsert_profile(
            umi_studio_toolchain_centre_catalogue(centre), &clang_profile) !=
            UMI_STATUS_OK) {
        umi_studio_toolchain_centre_destroy(centre);
        return EXIT_FAILURE;
    }
    binding.struct_size = (uint32_t)sizeof(binding);
    binding.api_version = UMI_TOOLCHAIN_CATALOGUE_API_VERSION;
    (void)strcpy(binding.project_id, "framework");
    (void)strcpy(binding.profile_id, "fixture-gcc");
    (void)strcpy(binding.compilation_database,
                 "build/framework/compile_commands.json");
    if (umi_studio_toolchain_centre_bind_project(centre, &binding) !=
            UMI_STATUS_OK ||
        umi_studio_toolchain_centre_select_project(
            centre, "framework", 1) != UMI_STATUS_OK) {
        umi_studio_toolchain_centre_destroy(centre);
        return EXIT_FAILURE;
    }
    selected = umi_studio_toolchain_centre_selected_profile(centre);
    if (selected == NULL || strcmp(selected->profile_id, "fixture-gcc") != 0) {
        umi_studio_toolchain_centre_destroy(centre);
        return EXIT_FAILURE;
    }
    umi_studio_toolchain_centre_destroy(centre);
    return EXIT_SUCCESS;
}
