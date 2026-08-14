/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/dependency_centre.h
 *
 * PURPOSE:
 *   Classify runtime files that a Studio portable bundle must include.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_DEPENDENCY_CENTRE_H
#define UMICOM_STUDIO_DEPENDENCY_CENTRE_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioDependencyCentre {
    UmiRuntimeDependencyReport report;
} UmiStudioDependencyCentre;

void umi_studio_dependency_centre_init(UmiStudioDependencyCentre *centre);
UmiStatus umi_studio_dependency_centre_add(
    UmiStudioDependencyCentre *centre,
    const char *name,
    const char *resolved_path);
UmiDependencyDisposition umi_studio_dependency_centre_classify(
    const char *name);
int umi_studio_dependency_centre_ready(
    const UmiStudioDependencyCentre *centre);

#ifdef __cplusplus
}
#endif
#endif
