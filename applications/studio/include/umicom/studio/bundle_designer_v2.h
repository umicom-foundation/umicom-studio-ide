/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/bundle_designer_v2.h
 *
 * PURPOSE:
 *   Compose Studio package files and runtime dependencies into a portable bundle.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_BUNDLE_DESIGNER_V2_H
#define UMICOM_STUDIO_BUNDLE_DESIGNER_V2_H

#include "umicom/studio/dependency_centre_v2.h"
#include "umicom/studio/package_project_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioBundleDesignerV2 {
    UmiPortableBundlePlanV2 plan;
} UmiStudioBundleDesignerV2;

UmiStatus umi_studio_bundle_designer_v2_init(
    UmiStudioBundleDesignerV2 *designer,
    const UmiPackageManifestV2 *manifest,
    const char *platform_id,
    const char *staging_root,
    const char *output_path);
UmiStatus umi_studio_bundle_designer_v2_apply_dependencies(
    UmiStudioBundleDesignerV2 *designer,
    const UmiStudioDependencyCentreV2 *dependencies);
int umi_studio_bundle_designer_v2_ready(
    const UmiStudioBundleDesignerV2 *designer);

#ifdef __cplusplus
}
#endif
#endif
