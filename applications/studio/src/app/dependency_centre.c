/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/dependency_centre.c
 *
 * PURPOSE:
 *   Classify runtime files that a Studio portable bundle must include.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/dependency_centre.h"
#include <string.h>

void umi_studio_dependency_centre_init(UmiStudioDependencyCentre *centre)
{
    if (centre != NULL) umi_runtime_dependency_report_init(&centre->report);
}

UmiDependencyDisposition umi_studio_dependency_centre_classify(
    const char *name)
{
    static const char *system_dependencies[] = {
        "kernel32.dll", "user32.dll", "advapi32.dll", "shell32.dll",
        "ntdll.dll", "libc.so.6", "libm.so.6", "libpthread.so.0"
    };
    size_t index;
    if (name == NULL) return UMI_DEPENDENCY_EXCLUDE;
    for (index = 0U;
         index < sizeof(system_dependencies) / sizeof(system_dependencies[0]);
         ++index) {
        if (strcmp(name, system_dependencies[index]) == 0) {
            return UMI_DEPENDENCY_SYSTEM;
        }
    }
    return UMI_DEPENDENCY_BUNDLE;
}

UmiStatus umi_studio_dependency_centre_add(
    UmiStudioDependencyCentre *centre,
    const char *name,
    const char *resolved_path)
{
    UmiDependencyDisposition disposition;
    int resolved;
    if (centre == NULL || name == NULL || resolved_path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    disposition = umi_studio_dependency_centre_classify(name);
    resolved = disposition != UMI_DEPENDENCY_BUNDLE ||
               resolved_path[0] != '\0';
    return umi_runtime_dependency_report_add(
        &centre->report, name, resolved_path, disposition, resolved);
}

int umi_studio_dependency_centre_ready(
    const UmiStudioDependencyCentre *centre)
{
    return centre != NULL &&
           umi_runtime_dependency_report_ready(&centre->report);
}
