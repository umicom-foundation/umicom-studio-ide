/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/version.h
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_VERSION_H
#define UMICOM_STUDIO_VERSION_H

#include "umicom/base/version.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMICOM_STUDIO_VERSION_MAJOR 0
#define UMICOM_STUDIO_VERSION_MINOR 11
#define UMICOM_STUDIO_VERSION_PATCH 1
#define UMICOM_STUDIO_VERSION_STRING "0.11.1"

UmiVersion umi_studio_version(void);
const char *umi_studio_version_string(void);

#ifdef __cplusplus
}
#endif

#endif
