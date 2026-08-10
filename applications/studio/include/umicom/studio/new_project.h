/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/new_project.h
 *
 * PURPOSE:
 *   Generate starter .umiapp source for new Umicom applications before repository or toolkit-specific files are created.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Studio-facing contract composes reusable Framework capabilities.
 * Generic designer and declarative logic remains owned by Umicom Framework.
 */

#ifndef UMICOM_STUDIO_NEW_PROJECT_H
#define UMICOM_STUDIO_NEW_PROJECT_H
#include "umicom/studio/declarative.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiStudioProjectTemplate { UMI_STUDIO_TEMPLATE_DESKTOP=1, UMI_STUDIO_TEMPLATE_EDITOR=2, UMI_STUDIO_TEMPLATE_DASHBOARD=3 } UmiStudioProjectTemplate;
UmiStatus umi_studio_new_project_template(UmiStudioProjectTemplate template_kind,const char *application_id,char *out_source,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
