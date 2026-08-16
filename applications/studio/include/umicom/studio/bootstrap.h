/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/bootstrap.h
 *
 * PURPOSE:
 *   Own the Umicom Framework composition root shared by the console and GTK4
 *   frontends.  Studio services, commands and Slave Controllers are registered
 *   here, including access to retained diagnostics and runtime registries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_BOOTSTRAP_H
#define UMICOM_STUDIO_BOOTSTRAP_H

#include <stddef.h>

#include "umicom/umicom.h"
#include "umicom/studio/services.h"
#include "umicom/studio/ui.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioBootstrap UmiStudioBootstrap;

UmiStatus umi_studio_bootstrap_create(UmiStudioBootstrap **out_bootstrap);
UmiStatus umi_studio_bootstrap_start(UmiStudioBootstrap *bootstrap);
UmiStatus umi_studio_bootstrap_stop(UmiStudioBootstrap *bootstrap);
void umi_studio_bootstrap_destroy(UmiStudioBootstrap *bootstrap);

const char *umi_studio_bootstrap_application_name(
    const UmiStudioBootstrap *bootstrap
);

size_t umi_studio_bootstrap_module_count(
    const UmiStudioBootstrap *bootstrap
);

UmiStudioServices *umi_studio_bootstrap_services(
    UmiStudioBootstrap *bootstrap
);

UmiServiceRegistry *umi_studio_bootstrap_service_registry(
    UmiStudioBootstrap *bootstrap
);
UmiCommandRegistry *umi_studio_bootstrap_command_registry(
    UmiStudioBootstrap *bootstrap
);
UmiHealthRegistry *umi_studio_bootstrap_health_registry(
    UmiStudioBootstrap *bootstrap
);
UmiStudioUi *umi_studio_bootstrap_ui(UmiStudioBootstrap *bootstrap);
UmiDesktopRuntime *umi_studio_bootstrap_desktop_runtime(
    UmiStudioBootstrap *bootstrap);
UmiDesktopShellModel *umi_studio_bootstrap_desktop_shell(
    UmiStudioBootstrap *bootstrap);
UmiDesktopContentRuntime *umi_studio_bootstrap_desktop_content(
    UmiStudioBootstrap *bootstrap);
UmiUiComponentHostService *umi_studio_bootstrap_component_host(
    UmiStudioBootstrap *bootstrap);
UmiDesktopComponentDragDrop *umi_studio_bootstrap_component_drag_drop(
    UmiStudioBootstrap *bootstrap);
UmiDesktopContextSynchronizer *umi_studio_bootstrap_context_synchronizer(
    UmiStudioBootstrap *bootstrap);
UmiDesktopLayoutHistory *umi_studio_bootstrap_layout_history(
    UmiStudioBootstrap *bootstrap);
UmiDesktopSessionRecovery *umi_studio_bootstrap_session_recovery(
    UmiStudioBootstrap *bootstrap);
UmiDesktopMonitorInteraction *umi_studio_bootstrap_monitor_interaction(
    UmiStudioBootstrap *bootstrap);
#ifdef __cplusplus
}
#endif

#endif
