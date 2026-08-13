/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/ui.c
 *
 * PURPOSE:
 *   Construct and own the Studio UI composition, publish its Framework services,
 *   refresh view models and provide deterministic headless rendering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/studio/ui_catalogue.h"
#include "umicom/studio/view_models.h"
#include "umicom/studio/workbench.h"
#include "umicom/studio/workbench_commands.h"

struct UmiStudioUi {
    UmiStudioServices *services;
    UmiCommandRegistry *commands;
    UmiUiWorkbench *workbench;
    UmiUiApplicationShell *shell;
    UmiUiHeadlessAdapter *headless;
    UmiStudioViewModels *view_models;
    UmiDocumentCoordinator *document_coordinator;
    int published;
};

static void destroy_partial(UmiStudioUi *ui)
{
    if (ui == NULL) return;
    umi_document_coordinator_destroy(ui->document_coordinator);
    ui->document_coordinator = NULL;
    umi_ui_headless_adapter_destroy(ui->headless);
    ui->headless = NULL;
    umi_ui_application_shell_destroy(ui->shell);
    ui->shell = NULL;
    umi_studio_view_models_destroy(ui->view_models);
    ui->view_models = NULL;
    umi_ui_workbench_destroy(ui->workbench);
    ui->workbench = NULL;
    free(ui);
}

UmiStatus umi_studio_ui_create(UmiStudioServices *services,
                               UmiCommandRegistry *commands,
                               UmiStudioUi **out_ui)
{
    UmiStudioUi *ui;
    UmiStatus status;
    if (services == NULL || commands == NULL || out_ui == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_ui = NULL;
    ui = (UmiStudioUi *)calloc(1U, sizeof(*ui));
    if (ui == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    ui->services = services;
    ui->commands = commands;
    status = umi_ui_workbench_create(UMI_STUDIO_WORKBENCH_ID, commands, &ui->workbench);
    if (status == UMI_STATUS_OK) status = umi_studio_workbench_populate(ui->workbench, services);
    if (status == UMI_STATUS_OK) status = umi_document_coordinator_create(
        umi_studio_services_documents(services), ui->workbench, NULL,
        &ui->document_coordinator);
    if (status == UMI_STATUS_OK) status = umi_studio_workbench_commands_register(commands, ui);
    if (status == UMI_STATUS_OK) status = umi_document_commands_register(
        commands, ui->document_coordinator);
    if (status == UMI_STATUS_OK) status = umi_studio_view_models_create(services, &ui->view_models);
    if (status == UMI_STATUS_OK) status = umi_ui_application_shell_create(
        UMI_STUDIO_APPLICATION_ID, "Umicom Studio IDE", ui->workbench, &ui->shell);
    if (status == UMI_STATUS_OK) status = umi_ui_application_shell_set_bounds(
        ui->shell, (UmiUiRect){ 80, 60, 1440, 900 });
    if (status == UMI_STATUS_OK) status = umi_ui_application_shell_set_visible(ui->shell, 1);
    if (status == UMI_STATUS_OK) status = umi_ui_headless_adapter_create(&ui->headless);
    if (status == UMI_STATUS_OK) status = umi_ui_headless_adapter_present(ui->headless, ui->shell);
    if (status == UMI_STATUS_OK) status = umi_studio_workbench_restore_session(
        ui->workbench, umi_studio_services_session(services));
    if (status != UMI_STATUS_OK) {
        destroy_partial(ui);
        return status;
    }
    *out_ui = ui;
    return UMI_STATUS_OK;
}

void umi_studio_ui_destroy(UmiStudioUi *ui)
{
    if (ui == NULL) return;
    (void)umi_studio_workbench_save_session(ui->workbench,
                                            umi_studio_services_session(ui->services));
    destroy_partial(ui);
}

UmiStatus umi_studio_ui_publish(UmiStudioUi *ui, UmiServiceRegistry *registry)
{
    static const char *const IDS[] = {
        "umicom.studio.ui",
        "umicom.ui.workbench",
        "umicom.ui.application-shell",
        "umicom.ui.adapter.headless",
        "umicom.studio.ui.view-models",
        "umicom.document.coordinator"
    };
    void *instances[6];
    size_t index;
    UmiServiceDescriptor descriptor;
    UmiStatus status;
    if (ui == NULL || registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (ui->published) return UMI_STATUS_INVALID_STATE;
    instances[0] = ui;
    instances[1] = ui->workbench;
    instances[2] = ui->shell;
    instances[3] = ui->headless;
    instances[4] = ui->view_models;
    instances[5] = ui->document_coordinator;
    for (index = 0U; index < 6U; ++index) {
        (void)memset(&descriptor, 0, sizeof(descriptor));
        descriptor.structure_size = (uint32_t)sizeof(descriptor);
        descriptor.service_id = IDS[index];
        descriptor.service_version = (UmiVersion){1, 0, 0};
        descriptor.provider_module_id = "org.umicom.studio.shell";
        descriptor.service = instances[index];
        descriptor.flags = UMI_SERVICE_SINGLETON;
        status = umi_service_registry_register(registry, &descriptor);
        if (status != UMI_STATUS_OK) return status;
    }
    ui->published = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_ui_refresh(UmiStudioUi *ui)
{
    UmiStatus status;
    if (ui == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_studio_view_models_refresh(ui->view_models);
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_headless_adapter_refresh(ui->headless);
}

UmiStatus umi_studio_ui_snapshot(const UmiStudioUi *ui,
                                 UmiStudioUiSnapshot *out_snapshot)
{
    UmiUiWorkbenchSnapshot workbench;
    UmiUiHeadlessSnapshot render;
    UmiStatus status;
    if (ui == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_workbench_snapshot(ui->workbench, &workbench);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_headless_adapter_snapshot(ui->headless, &render);
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->perspectives = workbench.perspective_count;
    out_snapshot->panes = workbench.pane_count;
    out_snapshot->documents = workbench.document_count;
    out_snapshot->actions = workbench.action_count;
    out_snapshot->menus = umi_ui_menu_model_count(umi_ui_workbench_menus(ui->workbench));
    out_snapshot->toolbars = umi_ui_toolbar_model_count(umi_ui_workbench_toolbars(ui->workbench));
    out_snapshot->status_items = umi_ui_status_model_count(umi_ui_workbench_status(ui->workbench));
    out_snapshot->contributions = umi_ui_contribution_model_count(umi_ui_workbench_contributions(ui->workbench));
    out_snapshot->notifications = workbench.notification_count;
    out_snapshot->workbench_revision = workbench.revision;
    out_snapshot->render_revision = render.render_revision;
    (void)snprintf(out_snapshot->active_perspective, sizeof(out_snapshot->active_perspective), "%s", workbench.active_perspective);
    (void)snprintf(out_snapshot->active_document, sizeof(out_snapshot->active_document), "%s", workbench.active_document_view);
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_ui_render_headless(UmiStudioUi *ui,
                                        UmiUiHeadlessSnapshot *out_snapshot)
{
    UmiStatus status;
    if (ui == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_studio_ui_refresh(ui);
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_headless_adapter_snapshot(ui->headless, out_snapshot);
}

UmiStudioServices *umi_studio_ui_services(UmiStudioUi *ui) { return ui != NULL ? ui->services : NULL; }
UmiUiApplicationShell *umi_studio_ui_shell(UmiStudioUi *ui) { return ui != NULL ? ui->shell : NULL; }
UmiUiWorkbench *umi_studio_ui_workbench(UmiStudioUi *ui) { return ui != NULL ? ui->workbench : NULL; }
UmiUiHeadlessAdapter *umi_studio_ui_headless(UmiStudioUi *ui) { return ui != NULL ? ui->headless : NULL; }
UmiStudioViewModels *umi_studio_ui_view_models(UmiStudioUi *ui) { return ui != NULL ? ui->view_models : NULL; }
UmiDocumentCoordinator *umi_studio_ui_documents(UmiStudioUi *ui) { return ui != NULL ? ui->document_coordinator : NULL; }
