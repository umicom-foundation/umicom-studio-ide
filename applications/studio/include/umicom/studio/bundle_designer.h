/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/bundle_designer.h
 *
 * PURPOSE:
 *   Compose Studio package files and runtime dependencies into a portable bundle.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_BUNDLE_DESIGNER_H
#define UMICOM_STUDIO_BUNDLE_DESIGNER_H

#include "umicom/studio/dependency_centre.h"
#include "umicom/studio/package_project.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioBundleDesigner {
    UmiPortableBundlePlan plan;
} UmiStudioBundleDesigner;

UmiStatus umi_studio_bundle_designer_init(
    UmiStudioBundleDesigner *designer,
    const UmiPackageManifest *manifest,
    const char *platform_id,
    const char *staging_root,
    const char *output_path);
UmiStatus umi_studio_bundle_designer_apply_dependencies(
    UmiStudioBundleDesigner *designer,
    const UmiStudioDependencyCentre *dependencies);
int umi_studio_bundle_designer_ready(
    const UmiStudioBundleDesigner *designer);

#ifdef __cplusplus
}
#endif
#endif
