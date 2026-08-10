/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/workbench.h
 *
 * PURPOSE:
 *   Declare Studio workbench identity, default layout composition and reset
 *   operations without exposing GTK4 widget types.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_WORKBENCH_H
#define UMICOM_STUDIO_WORKBENCH_H

#include "umicom/umicom.h"
#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_WORKBENCH_ID "org.umicom.studio.workbench"
#define UMI_STUDIO_APPLICATION_ID "org.umicom.studio"
#define UMI_STUDIO_DEFAULT_PERSPECTIVE "studio.perspective.develop"

UmiStatus umi_studio_workbench_populate(UmiUiWorkbench *workbench,
                                        UmiStudioServices *services);
UmiStatus umi_studio_workbench_reset_layout(UmiUiWorkbench *workbench);
UmiStatus umi_studio_workbench_restore_session(UmiUiWorkbench *workbench,
                                               UmiSessionStore *session);
UmiStatus umi_studio_workbench_save_session(UmiUiWorkbench *workbench,
                                            UmiSessionStore *session);

#ifdef __cplusplus
}
#endif

#endif
