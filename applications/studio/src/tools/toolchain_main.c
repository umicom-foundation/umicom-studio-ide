/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/toolchain_main.c
 * PURPOSE: Native Toolchain & Environment Centre inspection command.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/toolchain_centre.h"

#include <stdio.h>
#include <string.h>

static void usage(void)
{
    (void)printf("Usage: umicom-studio-toolchain [--profile ID] "
                 "[--root PATH] [--compile-commands PATH] "
                 "[--project-root PATH] [--no-probe]\n");
}

int main(int argc, char **argv)
{
    UmiStudioToolchainCentre *centre = NULL;
    UmiToolchainCatalogueDiscoveryRequest request;
    UmiToolchainCatalogueDiscoverySnapshot discovery;
    UmiStudioToolchainCentreSnapshot snapshot;
    const char *root = NULL;
    const char *roots[1];
    const char *compilation_database = NULL;
    const char *project_root = NULL;
    char discovered_database[UMI_TOOL_PATH_CAPACITY];
    int index;
    (void)memset(&request, 0, sizeof(request));
    request.require_c23 = 1;
    request.run_compile_probe = 1;
    for (index = 1; index < argc; ++index) {
        if (strcmp(argv[index], "--profile") == 0 && index + 1 < argc)
            request.preferred_profile = argv[++index];
        else if (strcmp(argv[index], "--root") == 0 && index + 1 < argc)
            root = argv[++index];
        else if (strcmp(argv[index], "--compile-commands") == 0 &&
                 index + 1 < argc)
            compilation_database = argv[++index];
        else if (strcmp(argv[index], "--project-root") == 0 &&
                 index + 1 < argc)
            project_root = argv[++index];
        else if (strcmp(argv[index], "--no-probe") == 0) {
            /* A skipped compile probe cannot truthfully certify C23.  Keep
             * inventory-only mode useful by selecting a complete compiler
             * profile without claiming that the C23 execution gate passed. */
            request.run_compile_probe = 0;
            request.require_c23 = 0;
        }
        else {
            usage();
            return 2;
        }
    }
    if (root != NULL) {
        roots[0] = root;
        request.explicit_roots = roots;
        request.explicit_root_count = 1U;
    }
    if (umi_studio_toolchain_centre_create(&centre) != UMI_STATUS_OK) return 1;
    if (umi_studio_toolchain_centre_discover(
            centre, &request, &discovery) != UMI_STATUS_OK) {
        (void)fprintf(stderr, "No compatible C23 toolchain was discovered.\n");
        umi_studio_toolchain_centre_destroy(centre);
        return 1;
    }
    if (compilation_database != NULL &&
        umi_studio_toolchain_centre_load_compilation_database(
            centre, compilation_database) != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Could not load %s\n", compilation_database);
        umi_studio_toolchain_centre_destroy(centre);
        return 1;
    }
    if (compilation_database == NULL && project_root != NULL &&
        umi_studio_toolchain_centre_discover_compilation_database(
            centre, project_root, NULL, 0U, discovered_database,
            sizeof(discovered_database)) != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Could not discover compile_commands.json below %s\n",
                      project_root);
        umi_studio_toolchain_centre_destroy(centre);
        return 1;
    }
    if (umi_studio_toolchain_centre_snapshot(centre, &snapshot) != UMI_STATUS_OK) {
        umi_studio_toolchain_centre_destroy(centre);
        return 1;
    }
    (void)printf("Toolchain profiles: %zu\n", snapshot.catalogue.profile_count);
    (void)printf("Complete profiles: %zu\n",
                 snapshot.catalogue.complete_profile_count);
    (void)printf("C23 profiles: %zu\n", snapshot.catalogue.c23_profile_count);
    (void)printf("Selected profile: %s\n",
                 snapshot.selected_capability.profile_id);
    (void)printf("Compiler: %s %u.%u.%u\n",
                 umi_compiler_vendor_text(
                     snapshot.selected_capability.compiler_vendor),
                 snapshot.selected_capability.compiler_version_major,
                 snapshot.selected_capability.compiler_version_minor,
                 snapshot.selected_capability.compiler_version_patch);
    (void)printf("Target: %s\n",
                 snapshot.selected_capability.target_triple[0] != '\0'
                     ? snapshot.selected_capability.target_triple : "unknown");
    (void)printf("CMake/Ninja/pkg-config: %s/%s/%s\n",
                 snapshot.selected_capability.cmake_available ? "PASS" : "FAIL",
                 snapshot.selected_capability.ninja_available ? "PASS" : "FAIL",
                 snapshot.selected_capability.pkg_config_available ? "PASS" : "N/A");
    (void)printf("Child environment entries: %zu\n",
                 snapshot.environment_variable_count);
    if (snapshot.has_compilation_database)
        (void)printf("Compilation commands: %zu\n",
                     snapshot.compilation_database.command_count);
    umi_studio_toolchain_centre_destroy(centre);
    return 0;
}
