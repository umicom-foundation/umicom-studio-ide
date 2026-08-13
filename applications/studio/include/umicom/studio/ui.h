/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/ui.h
 *
 * PURPOSE:
 *   Own the toolkit-neutral Studio UI composition built from Umicom Framework
 *   workbench, application-shell, headless-adapter and command contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_UI_H
#define UMICOM_STUDIO_UI_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/umicom.h"
#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioUi UmiStudioUi;
typedef struct UmiStudioViewModels UmiStudioViewModels;

typedef struct UmiStudioUiSnapshot {
    size_t perspectives;
    size_t panes;
    size_t documents;
    size_t actions;
    size_t menus;
    size_t toolbars;
    size_t status_items;
    size_t contributions;
    size_t notifications;
    uint64_t workbench_revision;
    uint64_t render_revision;
    char active_perspective[UMI_UI_ID_CAPACITY];
    char active_document[UMI_UI_ID_CAPACITY];
} UmiStudioUiSnapshot;

UmiStatus umi_studio_ui_create(UmiStudioServices *services,
                               UmiCommandRegistry *commands,
                               UmiStudioUi **out_ui);
void umi_studio_ui_destroy(UmiStudioUi *ui);
UmiStatus umi_studio_ui_publish(UmiStudioUi *ui,
                                UmiServiceRegistry *registry);
UmiStatus umi_studio_ui_refresh(UmiStudioUi *ui);
UmiStatus umi_studio_ui_snapshot(const UmiStudioUi *ui,
                                 UmiStudioUiSnapshot *out_snapshot);
UmiStatus umi_studio_ui_render_headless(UmiStudioUi *ui,
                                        UmiUiHeadlessSnapshot *out_snapshot);

UmiStudioServices *umi_studio_ui_services(UmiStudioUi *ui);
UmiUiApplicationShell *umi_studio_ui_shell(UmiStudioUi *ui);
UmiUiWorkbench *umi_studio_ui_workbench(UmiStudioUi *ui);
UmiUiHeadlessAdapter *umi_studio_ui_headless(UmiStudioUi *ui);
UmiStudioViewModels *umi_studio_ui_view_models(UmiStudioUi *ui);
UmiDocumentCoordinator *umi_studio_ui_documents(UmiStudioUi *ui);

#ifdef __cplusplus
}
#endif

#endif
