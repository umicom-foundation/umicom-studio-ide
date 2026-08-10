/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/perspectives.h
 *
 * PURPOSE:
 *   Declare the canonical Studio perspectives shared by headless, GTK4 and
 *   future Umicom frontend adapters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_PERSPECTIVES_H
#define UMICOM_STUDIO_PERSPECTIVES_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_PERSPECTIVE_DEVELOP "studio.perspective.develop"
#define UMI_STUDIO_PERSPECTIVE_BUILD "studio.perspective.build"
#define UMI_STUDIO_PERSPECTIVE_TEST "studio.perspective.test"
#define UMI_STUDIO_PERSPECTIVE_DEBUG "studio.perspective.debug"
#define UMI_STUDIO_PERSPECTIVE_SOURCE_CONTROL "studio.perspective.source-control"
#define UMI_STUDIO_PERSPECTIVE_AI "studio.perspective.ai"
#define UMI_STUDIO_PERSPECTIVE_DESIGNER "studio.perspective.designer"
#define UMI_STUDIO_PERSPECTIVE_RELEASE "studio.perspective.release"

UmiStatus umi_studio_perspectives_register(UmiUiWorkbench *workbench);
size_t umi_studio_perspective_definition_count(void);

#ifdef __cplusplus
}
#endif

#endif
