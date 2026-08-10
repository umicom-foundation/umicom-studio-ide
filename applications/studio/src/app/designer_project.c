/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/designer_project.c
 *
 * PURPOSE:
 *   Wrap the reusable Framework designer project model for Studio composition without adding duplicate state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Studio adapter keeps product composition thin: it delegates reusable
 * behaviour to Framework services and adds only Studio-specific coordination.
 */

#include "umicom/studio/designer_project.h"
UmiStatus umi_studio_designer_project_create(const char *id,UmiStudioDesignerProject *out){if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;out->project=NULL;return umi_designer_project_create(id,&out->project);}void umi_studio_designer_project_dispose(UmiStudioDesignerProject *p){if(p!=NULL){umi_designer_project_destroy(p->project);p->project=NULL;}}
