/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/core/bootstrap.c
 *
 * PURPOSE:
 *   Introduce Umicom Framework beneath the existing Studio implementation
 *   without rewriting the current GTK4 application in one unsafe step.
 *
 * MIGRATION STAGE:
 *   The first registered module is a compatibility module representing the
 *   preserved Studio shell.  Later commits replace it with Workspace,
 *   Document, Build, Output, Search and AI Slave Controllers.
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
    UmiModuleDescriptor legacy_shell_module;
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

static UmiStatus legacy_shell_configure(UmiModuleContext *context)
{
    return context != NULL ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

static UmiStatus legacy_shell_initialise(UmiModuleContext *context)
{
    return context != NULL ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

static UmiStatus legacy_shell_start(UmiModuleContext *context)
{
    return context != NULL ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

static UmiStatus legacy_shell_quiesce(UmiModuleContext *context)
{
    return context != NULL ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

static UmiStatus legacy_shell_stop(UmiModuleContext *context)
{
    return context != NULL ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

static void legacy_shell_destroy(UmiModuleContext *context)
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

    bootstrap->legacy_shell_module.structure_size =
        (uint32_t)sizeof(bootstrap->legacy_shell_module);
    bootstrap->legacy_shell_module.abi_version = UMICOM_FRAMEWORK_ABI_VERSION;
    bootstrap->legacy_shell_module.module_id = "org.umicom.studio.legacy-shell";
    bootstrap->legacy_shell_module.display_name = "Studio legacy GTK shell";
    bootstrap->legacy_shell_module.module_version = (UmiVersion){0U, 10U, 0U};
    bootstrap->legacy_shell_module.kind = UMI_MODULE_UI;
    bootstrap->legacy_shell_module.provided_capabilities = NULL;
    bootstrap->legacy_shell_module.required_capabilities = NULL;
    bootstrap->legacy_shell_module.module_state = bootstrap;
    bootstrap->legacy_shell_module.lifecycle.configure = legacy_shell_configure;
    bootstrap->legacy_shell_module.lifecycle.initialise = legacy_shell_initialise;
    bootstrap->legacy_shell_module.lifecycle.start = legacy_shell_start;
    bootstrap->legacy_shell_module.lifecycle.quiesce = legacy_shell_quiesce;
    bootstrap->legacy_shell_module.lifecycle.stop = legacy_shell_stop;
    bootstrap->legacy_shell_module.lifecycle.destroy = legacy_shell_destroy;

    status = umi_master_controller_register(
        bootstrap->master,
        &bootstrap->legacy_shell_module
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
