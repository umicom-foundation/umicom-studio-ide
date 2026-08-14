/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/dependency_centre_v2.h
 *
 * PURPOSE:
 *   Classify runtime files that a Studio portable bundle must include.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_DEPENDENCY_CENTRE_V2_H
#define UMICOM_STUDIO_DEPENDENCY_CENTRE_V2_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioDependencyCentreV2 {
    UmiRuntimeDependencyReportV2 report;
} UmiStudioDependencyCentreV2;

void umi_studio_dependency_centre_v2_init(UmiStudioDependencyCentreV2 *centre);
UmiStatus umi_studio_dependency_centre_v2_add(
    UmiStudioDependencyCentreV2 *centre,
    const char *name,
    const char *resolved_path);
UmiDependencyDispositionV2 umi_studio_dependency_centre_v2_classify(
    const char *name);
int umi_studio_dependency_centre_v2_ready(
    const UmiStudioDependencyCentreV2 *centre);

#ifdef __cplusplus
}
#endif
#endif
