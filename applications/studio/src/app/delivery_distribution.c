/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/delivery_distribution.c
 *
 * PURPOSE:
 *   Define Studio's supported distribution targets and runtime bundle defaults.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Distribution metadata tells the release system what Studio intends to ship on each supported platform.
 */

#include "umicom/studio/delivery_distribution.h"

UmiStatus umi_studio_distribution_matrix(UmiPlatformMatrix *out_matrix)
{
    if (out_matrix == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_platform_matrix_init(out_matrix);
    if (umi_platform_matrix_add(out_matrix, "windows", "x86_64") != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_platform_matrix_add(out_matrix, "linux", "x86_64");
}

UmiStatus umi_studio_runtime_bundle(UmiRuntimeBundle *out_bundle)
{
    if (out_bundle == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_runtime_bundle_init(out_bundle);
    if (umi_runtime_bundle_add(out_bundle, "bin/umicom-studio-ide") != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (umi_runtime_bundle_add(out_bundle, "bin/umicom") != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_runtime_bundle_add(out_bundle, "share/umicom");
}
