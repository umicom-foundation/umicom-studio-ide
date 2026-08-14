/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/dependency_centre_v2.c
 *
 * PURPOSE:
 *   Classify runtime files that a Studio portable bundle must include.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/dependency_centre_v2.h"
#include <string.h>

void umi_studio_dependency_centre_v2_init(UmiStudioDependencyCentreV2 *centre)
{
    if (centre != NULL) umi_runtime_dependency_report_v2_init(&centre->report);
}

UmiDependencyDispositionV2 umi_studio_dependency_centre_v2_classify(
    const char *name)
{
    static const char *system_dependencies[] = {
        "kernel32.dll", "user32.dll", "advapi32.dll", "shell32.dll",
        "ntdll.dll", "libc.so.6", "libm.so.6", "libpthread.so.0"
    };
    size_t index;
    if (name == NULL) return UMI_DEPENDENCY_EXCLUDE_V2;
    for (index = 0U;
         index < sizeof(system_dependencies) / sizeof(system_dependencies[0]);
         ++index) {
        if (strcmp(name, system_dependencies[index]) == 0) {
            return UMI_DEPENDENCY_SYSTEM_V2;
        }
    }
    return UMI_DEPENDENCY_BUNDLE_V2;
}

UmiStatus umi_studio_dependency_centre_v2_add(
    UmiStudioDependencyCentreV2 *centre,
    const char *name,
    const char *resolved_path)
{
    UmiDependencyDispositionV2 disposition;
    int resolved;
    if (centre == NULL || name == NULL || resolved_path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    disposition = umi_studio_dependency_centre_v2_classify(name);
    resolved = disposition != UMI_DEPENDENCY_BUNDLE_V2 ||
               resolved_path[0] != '\0';
    return umi_runtime_dependency_report_v2_add(
        &centre->report, name, resolved_path, disposition, resolved);
}

int umi_studio_dependency_centre_v2_ready(
    const UmiStudioDependencyCentreV2 *centre)
{
    return centre != NULL &&
           umi_runtime_dependency_report_v2_ready(&centre->report);
}
