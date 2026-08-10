/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/designer_project.h
 *
 * PURPOSE:
 *   Provide Studio project-level helpers for storing multiple reusable designer documents under one project identity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Studio-facing contract composes reusable Framework capabilities.
 * Generic designer and declarative logic remains owned by Umicom Framework.
 */

#ifndef UMICOM_STUDIO_DESIGNER_PROJECT_H
#define UMICOM_STUDIO_DESIGNER_PROJECT_H
#include "umicom/studio/designer.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioDesignerProject { UmiDesignerProject *project; } UmiStudioDesignerProject;
UmiStatus umi_studio_designer_project_create(const char *project_id,UmiStudioDesignerProject *out_project);
void umi_studio_designer_project_dispose(UmiStudioDesignerProject *project);
#ifdef __cplusplus
}
#endif
#endif
