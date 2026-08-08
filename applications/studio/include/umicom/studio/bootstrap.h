/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/bootstrap.h
 *
 * PURPOSE:
 *   Own the Umicom Framework composition root used by the existing Studio GUI.
 *   The preserved GTK implementation remains operational while shared services
 *   move into Framework one vertical slice at a time.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_BOOTSTRAP_H
#define UMICOM_STUDIO_BOOTSTRAP_H

#include <stddef.h>
#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioBootstrap UmiStudioBootstrap;

UmiStatus umi_studio_bootstrap_create(UmiStudioBootstrap **out_bootstrap);
UmiStatus umi_studio_bootstrap_start(UmiStudioBootstrap *bootstrap);
UmiStatus umi_studio_bootstrap_stop(UmiStudioBootstrap *bootstrap);
void umi_studio_bootstrap_destroy(UmiStudioBootstrap *bootstrap);

const char *umi_studio_bootstrap_application_name(const UmiStudioBootstrap *bootstrap);
size_t umi_studio_bootstrap_module_count(const UmiStudioBootstrap *bootstrap);

#ifdef __cplusplus
}
#endif

#endif
