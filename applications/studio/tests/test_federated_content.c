/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_federated_content.c
 *
 * PURPOSE:
 *   Verify Studio contributes thin view definitions while Framework owns
 *   component hosting and federated desktop window content.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/studio/bootstrap.h"

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiDesktopContentRuntime *content;
    UmiDesktopContentSnapshot snapshot;
    UmiUiViewPresentation *presentation =
        (UmiUiViewPresentation *)calloc(1U, sizeof(*presentation));
    assert(presentation != NULL);
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    content = umi_studio_bootstrap_desktop_content(bootstrap);
    assert(content != NULL);
    assert(umi_studio_bootstrap_component_host(bootstrap) != NULL);
    assert(umi_desktop_content_runtime_count(content) == 2U);
    assert(umi_desktop_content_runtime_snapshot(
        content, "project-explorer", &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.window.owner_application_id,
                  "org.umicom.studio") == 0);
    assert(strcmp(snapshot.host.descriptor.view_type,
                  "studio.project-explorer") == 0);
    assert(strcmp(snapshot.capability_id, "umicom.workspace") == 0);
    assert(umi_desktop_content_runtime_presentation(
        content, "project-explorer", presentation) == UMI_STATUS_OK);
    assert(strcmp(presentation->view.view_id,
                  "studio.federated.project-explorer") == 0);
    assert(umi_desktop_content_runtime_snapshot(
        content, "output", &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.host.descriptor.view_type, "studio.output") == 0);
    assert(umi_desktop_content_runtime_refresh(content, "output") ==
           UMI_STATUS_OK);
    assert(umi_desktop_content_runtime_activate(content, "project-explorer") ==
           UMI_STATUS_OK);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_stop(bootstrap) == UMI_STATUS_OK);
    umi_studio_bootstrap_destroy(bootstrap);
    free(presentation);
    return 0;
}
