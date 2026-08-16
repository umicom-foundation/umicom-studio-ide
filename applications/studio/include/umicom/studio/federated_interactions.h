/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/federated_interactions.h
 *
 * PURPOSE:
 *   Register Studio's declarative linked-context interests with the
 *   Framework-owned desktop context synchronizer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_FEDERATED_INTERACTIONS_H
#define UMICOM_STUDIO_FEDERATED_INTERACTIONS_H

#include "umicom/desktop/context_synchronizer.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_federated_interactions_register(
    UmiDesktopContextSynchronizer *synchronizer);

#ifdef __cplusplus
}
#endif

#endif
