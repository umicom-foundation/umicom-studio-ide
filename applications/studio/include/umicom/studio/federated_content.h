/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/federated_content.h
 *
 * PURPOSE:
 *   Contribute Studio view factories and declarative bindings to the
 *   Framework-owned federated window content runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_FEDERATED_CONTENT_H
#define UMICOM_STUDIO_FEDERATED_CONTENT_H

#include "umicom/desktop/content_runtime.h"
#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_federated_content_register(
    UmiDesktopContentRuntime *runtime,
    UmiStudioServices *services);

#ifdef __cplusplus
}
#endif

#endif
