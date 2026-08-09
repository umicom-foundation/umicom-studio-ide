/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/version.c
 *
 * PURPOSE:
 *   Implement the runtime Studio version API from the public version macros.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/version.h"

UmiVersion umi_studio_version(void)
{
    return (UmiVersion){UMICOM_STUDIO_VERSION_MAJOR,
                        UMICOM_STUDIO_VERSION_MINOR,
                        UMICOM_STUDIO_VERSION_PATCH};
}

const char *umi_studio_version_string(void)
{
    return UMICOM_STUDIO_VERSION_STRING;
}
