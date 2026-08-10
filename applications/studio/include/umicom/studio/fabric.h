/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/fabric.h
 *
 * PURPOSE:
 *   Declare Studio Integration Fabric bootstrap schemas, topics and health verification shared by product frontends, controllers and native inspection tools.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_FABRIC_H
#define UMICOM_STUDIO_FABRIC_H

#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_fabric_register_defaults(UmiStudioServices *services);
UmiStatus umi_studio_fabric_validate(UmiStudioServices *services);

#ifdef __cplusplus
}
#endif

#endif
