/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/package_inspector_main.c
 *
 * PURPOSE:
 *   Provide a native command for inspecting Studio package manifests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "umicom/studio/package_project.h"

int main(void)
{
    UmiStudioPackageProject project;
    if (umi_studio_package_project_init(&project, "0.23.0") !=
        UMI_STATUS_OK) return 1;
    printf("Umicom Studio Package Inspector\n");
    printf("Windows files: %zu\n", project.windows.count);
    printf("Linux files: %zu\n", project.linux.count);
    return umi_studio_package_project_ready(&project) ? 0 : 1;
}
