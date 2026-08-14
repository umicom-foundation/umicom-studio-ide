/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/bundle_designer_v2.c
 *
 * PURPOSE:
 *   Compose Studio package files and runtime dependencies into a portable bundle.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bundle_designer_v2.h"

UmiStatus umi_studio_bundle_designer_v2_init(
    UmiStudioBundleDesignerV2 *designer,
    const UmiPackageManifestV2 *manifest,
    const char *platform_id,
    const char *staging_root,
    const char *output_path)
{
    if (designer == NULL || manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_portable_bundle_v2_init(
        &designer->plan, "umicom-studio-portable", platform_id, staging_root,
        output_path, manifest);
}

UmiStatus umi_studio_bundle_designer_v2_apply_dependencies(
    UmiStudioBundleDesignerV2 *designer,
    const UmiStudioDependencyCentreV2 *dependencies)
{
    size_t index;
    UmiStatus status;
    if (designer == NULL || dependencies == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_runtime_dependency_report_v2_init(&designer->plan.dependencies);
    for (index = 0U; index < dependencies->report.count; ++index) {
        const UmiRuntimeDependencyV2 *dependency =
            &dependencies->report.dependencies[index];
        status = umi_portable_bundle_v2_add_dependency(
            &designer->plan, dependency->name, dependency->resolved_path,
            dependency->disposition, dependency->resolved);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

int umi_studio_bundle_designer_v2_ready(
    const UmiStudioBundleDesignerV2 *designer)
{
    return designer != NULL &&
           umi_portable_bundle_v2_validate(&designer->plan) == UMI_STATUS_OK;
}
