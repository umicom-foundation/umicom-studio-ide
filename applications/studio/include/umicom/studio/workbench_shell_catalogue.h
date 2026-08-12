/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/workbench_shell_catalogue.h
 *
 * PURPOSE:
 *   Declare Studio's product-specific composition of Framework Activity Bar, view-container, context, keybinding, Explorer and breadcrumb services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * All reusable behaviour remains in Framework. This file names Studio's specific activities and connects them to existing Studio panes.
 */

#ifndef UMICOM_STUDIO_WORKBENCH_SHELL_CATALOGUE_H
#define UMICOM_STUDIO_WORKBENCH_SHELL_CATALOGUE_H

#include "umicom/umicom.h"
#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_ACTIVITY_EXPLORER     "studio.activity.explorer"
#define UMI_STUDIO_ACTIVITY_SEARCH       "studio.activity.search"
#define UMI_STUDIO_ACTIVITY_SCM          "studio.activity.source-control"
#define UMI_STUDIO_ACTIVITY_RUN          "studio.activity.run-debug"
#define UMI_STUDIO_ACTIVITY_TEST         "studio.activity.testing"
#define UMI_STUDIO_ACTIVITY_DESIGNER     "studio.activity.designer"
#define UMI_STUDIO_ACTIVITY_APPLICATIONS "studio.activity.applications"
#define UMI_STUDIO_ACTIVITY_FRAMEWORK    "studio.activity.framework"
#define UMI_STUDIO_ACTIVITY_AI           "studio.activity.ai"

#define UMI_STUDIO_CONTAINER_EXPLORER     "studio.container.explorer"
#define UMI_STUDIO_CONTAINER_SEARCH       "studio.container.search"
#define UMI_STUDIO_CONTAINER_SCM          "studio.container.source-control"
#define UMI_STUDIO_CONTAINER_RUN          "studio.container.run-debug"
#define UMI_STUDIO_CONTAINER_TEST         "studio.container.testing"
#define UMI_STUDIO_CONTAINER_DESIGNER     "studio.container.designer"
#define UMI_STUDIO_CONTAINER_APPLICATIONS "studio.container.applications"
#define UMI_STUDIO_CONTAINER_FRAMEWORK    "studio.container.framework"
#define UMI_STUDIO_CONTAINER_AI           "studio.container.ai"

UmiStatus umi_studio_workbench_shell_catalogue_register(
    UmiUiWorkbench *workbench,
    UmiStudioServices *services
);

#ifdef __cplusplus
}
#endif
#endif
