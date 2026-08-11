/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/delivery_package.h
 *
 * PURPOSE:
 *   Create Studio package specifications and invoke the built-in directory-package reference provider.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This is the product adapter that supplies Studio paths and names while Framework owns package-provider contracts.
 */

#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_DELIVERY_PACKAGE_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_DELIVERY_PACKAGE_H

#include "umicom/umicom.h"
#include "umicom/studio/delivery_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_package_spec(const char *staging_root,
                                    const char *output_path,
                                    UmiPackageSpec *out_spec);
UmiStatus umi_studio_package_directory(const UmiPackageSpec *spec,
                                       UmiPackageResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
