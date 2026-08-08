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

#include <stdio.h>
#include <stdlib.h>

struct UmiStudioBootstrap {
    UmiMasterController *master;
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

    config.application_name = "Umicom Studio IDE";
    config.diagnostic_sink = studio_diagnostic_sink;
    config.diagnostic_user_data = bootstrap;

    status = umi_master_controller_create(&config, &bootstrap->master);
    if (status != UMI_STATUS_OK) {
        free(bootstrap);
        return status;
    }

    bootstrap->studio_shell_module.structure_size =
        (uint32_t)sizeof(bootstrap->studio_shell_module);
    bootstrap->studio_shell_module.abi_version = UMICOM_FRAMEWORK_ABI_VERSION;
    bootstrap->studio_shell_module.module_id = "org.umicom.studio.shell";
    bootstrap->studio_shell_module.display_name = "Umicom Studio product shell";
    bootstrap->studio_shell_module.module_version = (UmiVersion){0U, 11U, 0U};
    bootstrap->studio_shell_module.kind = UMI_MODULE_UI;
    bootstrap->studio_shell_module.provided_capabilities = NULL;
    bootstrap->studio_shell_module.required_capabilities = NULL;
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
        umi_master_controller_destroy(bootstrap->master);
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
        bootstrap->started = 1;
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
    }
    return status;
}

void umi_studio_bootstrap_destroy(UmiStudioBootstrap *bootstrap)
{
    if (bootstrap == NULL) {
        return;
    }
    (void)umi_studio_bootstrap_stop(bootstrap);
    umi_master_controller_destroy(bootstrap->master);
    bootstrap->master = NULL;
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
