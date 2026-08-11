/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/runtime-manager/include/umicom/studio/runtime_catalogue.h
 *
 * PURPOSE:
 *   Populate the Studio Application Hub with the known Umicom product catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is part of the Studio Application Hub / Runtime Manager.
 * The code is intentionally explicit and bounded so a new C developer can
 * follow the data flow without hidden ownership or private product coupling.
 */

#ifndef UMICOM_STUDIO_RUNTIME_CATALOGUE_H
#define UMICOM_STUDIO_RUNTIME_CATALOGUE_H

#include "umicom/base/status.h"
#include "umicom/studio/runtime_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_runtime_catalogue_populate(
    UmiStudioRuntimeManager *manager
);

#ifdef __cplusplus
}
#endif

#endif
