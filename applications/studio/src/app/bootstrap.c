/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/bootstrap.c
 *
 * PURPOSE:
 *   Own the Umicom Framework composition root used by every Studio frontend.
 *   The Studio product shell is registered with the Framework Master
 *   Controller and will be decomposed into bounded Slave Controllers as the
 *   migration continues.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/commands.h"
#include "umicom/studio/federated_content.h"
#include "umicom/studio/federated_interactions.h"
#include "umicom/studio/federated_workspace_session.h"
#include "umicom/studio/services.h"
#include "umicom/studio/ui.h"
#include "umicom/studio/version.h"

#include <stdio.h>
#include <stdlib.h>

struct UmiStudioBootstrap {
    UmiMasterController *master;
    UmiStudioServices *services;
    UmiStudioUi *ui;
    UmiModuleDescriptor studio_shell_module;
    int started;
};

static void studio_diagnostic_sink(const UmiDiagnostic *diagnostic, void *user_data)
{
    (void)user_data;
    if (diagnostic == NULL) {
        return;
    }
    (void)fprintf(stderr,
                  "[Studio Framework][%s][%s] %s\n",
                  umi_diagnostic_severity_text(diagnostic->severity),
                  diagnostic->source != NULL ? diagnostic->source : "unknown",
                  diagnostic->message != NULL ? diagnostic->message : "");
}

static UmiStatus studio_shell_configure(UmiModuleContext *context)
{
    return context != NULL ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

static UmiStatus studio_shell_initialise(UmiModuleContext *context)
{
    return context != NULL ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

static UmiStatus studio_shell_start(UmiModuleContext *context)
{
    return context != NULL ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

static UmiStatus studio_shell_quiesce(UmiModuleContext *context)
{
    return context != NULL ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

static UmiStatus studio_shell_stop(UmiModuleContext *context)
{
    return context != NULL ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

static void studio_shell_destroy(UmiModuleContext *context)
{
    (void)context;
}

UmiStatus umi_studio_bootstrap_create(UmiStudioBootstrap **out_bootstrap)
{
    UmiStudioBootstrap *bootstrap;
    UmiMasterControllerConfig config;
    UmiStatus status;

    if (out_bootstrap == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_bootstrap = NULL;

    bootstrap = (UmiStudioBootstrap *)calloc(1U, sizeof(*bootstrap));
    if (bootstrap == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    status = umi_studio_services_create(studio_diagnostic_sink, bootstrap,
                                        &bootstrap->services);
    if (status != UMI_STATUS_OK) {
        free(bootstrap);
        return status;
    }

    config.application_name = "Umicom Studio IDE";
    config.diagnostic_sink = umi_studio_services_diagnostic_sink();
    config.diagnostic_user_data =
        umi_studio_services_diagnostic_user_data(bootstrap->services);

    status = umi_master_controller_create(&config, &bootstrap->master);
    if (status != UMI_STATUS_OK) {
        umi_studio_services_destroy(bootstrap->services);
        free(bootstrap);
        return status;
    }

    status = umi_master_controller_install_application_authority(
        bootstrap->master, "org.umicom.studio");
    if (status != UMI_STATUS_OK) {
        umi_master_controller_destroy(bootstrap->master);
        umi_studio_services_destroy(bootstrap->services);
        free(bootstrap);
        return status;
    }

    status = umi_master_controller_install_desktop_authority(
        bootstrap->master);
    if (status != UMI_STATUS_OK) {
        umi_master_controller_destroy(bootstrap->master);
        umi_studio_services_destroy(bootstrap->services);
        free(bootstrap);
        return status;
    }

    status = umi_studio_services_publish(bootstrap->services,
                                         bootstrap->master);
    if (status != UMI_STATUS_OK) {
        umi_master_controller_destroy(bootstrap->master);
        umi_studio_services_destroy(bootstrap->services);
        free(bootstrap);
        return status;
    }

    status = umi_studio_commands_register(
        umi_master_controller_command_registry(bootstrap->master),
        bootstrap->services
    );
    if (status != UMI_STATUS_OK) {
        umi_master_controller_destroy(bootstrap->master);
        umi_studio_services_destroy(bootstrap->services);
        free(bootstrap);
        return status;
    }

    status = umi_studio_ui_create(
        bootstrap->services,
        umi_master_controller_command_registry(bootstrap->master),
        &bootstrap->ui
    );
    if (status == UMI_STATUS_OK) {
        status = umi_studio_federated_content_register(
            umi_master_controller_desktop_content(bootstrap->master),
            bootstrap->services);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_federated_interactions_register(
            umi_master_controller_desktop_context_synchronizer(
                bootstrap->master));
    }
    if (status == UMI_STATUS_OK) {
        status = umi_studio_ui_publish(
            bootstrap->ui,
            umi_master_controller_services(bootstrap->master)
        );
    }
    if (status != UMI_STATUS_OK) {
        umi_studio_ui_destroy(bootstrap->ui);
        umi_master_controller_destroy(bootstrap->master);
        umi_studio_services_destroy(bootstrap->services);
        free(bootstrap);
        return status;
    }

    bootstrap->studio_shell_module.structure_size =
        (uint32_t)sizeof(bootstrap->studio_shell_module);
    bootstrap->studio_shell_module.abi_version = UMICOM_FRAMEWORK_ABI_VERSION;
    bootstrap->studio_shell_module.module_id = "org.umicom.studio.shell";
    bootstrap->studio_shell_module.display_name = "Umicom Studio product shell";
    bootstrap->studio_shell_module.module_version = umi_studio_version();
    bootstrap->studio_shell_module.kind = UMI_MODULE_UI;
    {
        static const char *provided_capabilities[] = {
            "umicom.studio.shell",
            "umicom.studio.workbench",
            "umicom.studio.data",
            "umicom.studio.integration-fabric",
            NULL
        };
        static const char *required_capabilities[] = {
            "umicom.diagnostics",
            "umicom.configuration",
            "umicom.runtime.capabilities",
            "umicom.security.policy",
            "umicom.application.federation",
            "umicom.application.context",
            "umicom.application.resources",
            "umicom.desktop",
            "umicom.desktop.layouts",
            "umicom.desktop.context",
            "umicom.desktop.shell",
            "umicom.desktop.designer",
            "umicom.desktop.content",
            "umicom.desktop.component-host",
            "umicom.desktop.view-factories",
            "umicom.desktop.component-drag-drop",
            "umicom.desktop.context-synchronizer",
            "umicom.desktop.layout-history",
            "umicom.desktop.session-recovery",
            "umicom.desktop.monitor-interaction",
            NULL
        };
        static const char *optional_capabilities[] = {
            "umicom.filesystem",
            "umicom.process",
            "umicom.data",
            "umicom.messaging.events",
            NULL
        };
        static const char *requested_permissions[] = {
            "filesystem.read",
            "filesystem.write.workspace",
            "process.execute.build-tools",
            NULL
        };
        bootstrap->studio_shell_module.provided_capabilities =
            provided_capabilities;
        bootstrap->studio_shell_module.required_capabilities =
            required_capabilities;
        bootstrap->studio_shell_module.optional_capabilities =
            optional_capabilities;
        bootstrap->studio_shell_module.requested_permissions =
            requested_permissions;
    }
    bootstrap->studio_shell_module.module_state = bootstrap;
    bootstrap->studio_shell_module.lifecycle.configure = studio_shell_configure;
    bootstrap->studio_shell_module.lifecycle.initialise = studio_shell_initialise;
    bootstrap->studio_shell_module.lifecycle.start = studio_shell_start;
    bootstrap->studio_shell_module.lifecycle.quiesce = studio_shell_quiesce;
    bootstrap->studio_shell_module.lifecycle.stop = studio_shell_stop;
    bootstrap->studio_shell_module.lifecycle.destroy = studio_shell_destroy;

    status = umi_master_controller_register(
        bootstrap->master,
        &bootstrap->studio_shell_module
    );
    if (status != UMI_STATUS_OK) {
        umi_studio_ui_destroy(bootstrap->ui);
        umi_master_controller_destroy(bootstrap->master);
        umi_studio_services_destroy(bootstrap->services);
        free(bootstrap);
        return status;
    }

    *out_bootstrap = bootstrap;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_bootstrap_start(UmiStudioBootstrap *bootstrap)
{
    UmiStatus status;
    if (bootstrap == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (bootstrap->started) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_master_controller_start(bootstrap->master);
    if (status == UMI_STATUS_OK) {
        status = umi_studio_federated_workspace_session_begin(
            umi_master_controller_desktop_session_recovery(
                bootstrap->master),
            umi_master_controller_desktop_monitor_interaction(
                bootstrap->master));
        if (status == UMI_STATUS_OK) {
            bootstrap->started = 1;
        } else {
            (void)umi_master_controller_stop(bootstrap->master);
        }
    }
    return status;
}

UmiStatus umi_studio_bootstrap_stop(UmiStudioBootstrap *bootstrap)
{
    UmiStatus status;
    if (bootstrap == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!bootstrap->started) {
        return UMI_STATUS_OK;
    }
    status = umi_master_controller_stop(bootstrap->master);
    if (status == UMI_STATUS_OK) {
        bootstrap->started = 0;
        status = umi_studio_federated_workspace_session_end(
            umi_master_controller_desktop_session_recovery(
                bootstrap->master));
    }
    return status;
}

void umi_studio_bootstrap_destroy(UmiStudioBootstrap *bootstrap)
{
    if (bootstrap == NULL) {
        return;
    }
    (void)umi_studio_bootstrap_stop(bootstrap);
    umi_studio_ui_destroy(bootstrap->ui);
    bootstrap->ui = NULL;
    umi_master_controller_destroy(bootstrap->master);
    bootstrap->master = NULL;
    umi_studio_services_destroy(bootstrap->services);
    bootstrap->services = NULL;
    free(bootstrap);
}

const char *umi_studio_bootstrap_application_name(const UmiStudioBootstrap *bootstrap)
{
    if (bootstrap == NULL || bootstrap->master == NULL) {
        return NULL;
    }
    return umi_master_controller_application_name(bootstrap->master);
}

size_t umi_studio_bootstrap_module_count(const UmiStudioBootstrap *bootstrap)
{
    if (bootstrap == NULL || bootstrap->master == NULL) {
        return 0U;
    }
    return umi_master_controller_module_count(bootstrap->master);
}

UmiStudioServices *umi_studio_bootstrap_services(UmiStudioBootstrap *bootstrap)
{
    return bootstrap != NULL ? bootstrap->services : NULL;
}

UmiServiceRegistry *umi_studio_bootstrap_service_registry(
    UmiStudioBootstrap *bootstrap)
{
    return bootstrap != NULL
        ? umi_master_controller_services(bootstrap->master)
        : NULL;
}

UmiCommandRegistry *umi_studio_bootstrap_command_registry(
    UmiStudioBootstrap *bootstrap)
{
    return bootstrap != NULL
        ? umi_master_controller_command_registry(bootstrap->master)
        : NULL;
}

UmiHealthRegistry *umi_studio_bootstrap_health_registry(
    UmiStudioBootstrap *bootstrap)
{
    return bootstrap != NULL
        ? umi_master_controller_health(bootstrap->master)
        : NULL;
}

UmiStudioUi *umi_studio_bootstrap_ui(UmiStudioBootstrap *bootstrap)
{
    return bootstrap != NULL ? bootstrap->ui : NULL;
}

UmiDesktopRuntime *umi_studio_bootstrap_desktop_runtime(
    UmiStudioBootstrap *bootstrap)
{
    return bootstrap != NULL
        ? umi_master_controller_desktop_runtime(bootstrap->master)
        : NULL;
}

UmiDesktopShellModel *umi_studio_bootstrap_desktop_shell(
    UmiStudioBootstrap *bootstrap)
{
    return bootstrap != NULL
        ? umi_master_controller_desktop_shell(bootstrap->master)
        : NULL;
}

UmiDesktopContentRuntime *umi_studio_bootstrap_desktop_content(
    UmiStudioBootstrap *bootstrap)
{
    return bootstrap != NULL
        ? umi_master_controller_desktop_content(bootstrap->master)
        : NULL;
}

UmiUiComponentHostService *umi_studio_bootstrap_component_host(
    UmiStudioBootstrap *bootstrap)
{
    return bootstrap != NULL
        ? umi_master_controller_desktop_component_host(bootstrap->master)
        : NULL;
}

UmiDesktopComponentDragDrop *umi_studio_bootstrap_component_drag_drop(
    UmiStudioBootstrap *bootstrap)
{
    return bootstrap != NULL
        ? umi_master_controller_desktop_component_drag_drop(bootstrap->master)
        : NULL;
}

UmiDesktopContextSynchronizer *umi_studio_bootstrap_context_synchronizer(
    UmiStudioBootstrap *bootstrap)
{
    return bootstrap != NULL
        ? umi_master_controller_desktop_context_synchronizer(bootstrap->master)
        : NULL;
}

UmiDesktopLayoutHistory *umi_studio_bootstrap_layout_history(
    UmiStudioBootstrap *bootstrap)
{
    return bootstrap != NULL
        ? umi_master_controller_desktop_layout_history(bootstrap->master)
        : NULL;
}

UmiDesktopSessionRecovery *umi_studio_bootstrap_session_recovery(
    UmiStudioBootstrap *bootstrap)
{
    return bootstrap != NULL
        ? umi_master_controller_desktop_session_recovery(bootstrap->master)
        : NULL;
}

UmiDesktopMonitorInteraction *umi_studio_bootstrap_monitor_interaction(
    UmiStudioBootstrap *bootstrap)
{
    return bootstrap != NULL
        ? umi_master_controller_desktop_monitor_interaction(bootstrap->master)
        : NULL;
}
