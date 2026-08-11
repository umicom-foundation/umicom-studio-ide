/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/delivery_package.c
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

#include "umicom/studio/delivery_package.h"

UmiStatus umi_studio_package_spec(const char *staging_root,
                                  const char *output_path,
                                  UmiPackageSpec *out_spec)
{
    return umi_package_spec_init(out_spec,
                                 "umicom-studio-portable",
                                 UMI_PACKAGE_DIRECTORY,
                                 staging_root,
                                 output_path);
}

UmiStatus umi_studio_package_directory(const UmiPackageSpec *spec,
                                       UmiPackageResult *out_result)
{
    UmiPackageProvider provider;
    UmiStatus status = umi_directory_package_provider(&provider);
    if (status != UMI_STATUS_OK) return status;
    return umi_package_provider_create(&provider, spec, out_result);
}
