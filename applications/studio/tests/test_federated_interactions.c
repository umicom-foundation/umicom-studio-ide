/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_federated_interactions.c
 *
 * PURPOSE:
 *   Verify Studio contributes declarations while Framework executes component
 *   drag/drop and linked-context synchronization.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/studio/bootstrap.h"

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiDesktopRuntime *desktop;
    UmiDesktopComponentDragDrop *drag_drop;
    UmiDesktopContextSynchronizer *synchronizer;
    UmiDesktopComponentDropTarget target = {0};
    UmiDesktopContextSynchronizerSnapshot context;
    const UmiDesktopWindow *window;

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    desktop = umi_studio_bootstrap_desktop_runtime(bootstrap);
    drag_drop = umi_studio_bootstrap_component_drag_drop(bootstrap);
    synchronizer = umi_studio_bootstrap_context_synchronizer(bootstrap);
    assert(desktop != NULL && drag_drop != NULL && synchronizer != NULL);
    assert(umi_desktop_context_synchronizer_subscription_count(
        synchronizer) == 5U);
    assert(umi_desktop_runtime_clone_layout(
        desktop, "develop", "studio-interactions", "Studio Interactions",
        true) == UMI_STATUS_OK);

    assert(umi_desktop_component_drag_begin(
        drag_drop, "studio.drag.project", "project-explorer",
        UMI_DESKTOP_COMPONENT_DRAG_MOVE) == UMI_STATUS_OK);
    target.structure_size = (uint32_t)sizeof(target);
    (void)snprintf(target.monitor_id, sizeof(target.monitor_id), "%s", "primary");
    target.bounds = (UmiDesktopRect){80, 80, 420, 760};
    target.placement = UMI_DESKTOP_DOCK_FLOATING;
    assert(umi_desktop_component_drag_target(
        drag_drop, "studio.drag.project", &target) == UMI_STATUS_OK);
    assert(umi_desktop_component_drag_commit(
        drag_drop, "studio.drag.project") == UMI_STATUS_OK);
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(desktop), "project-explorer");
    assert(window != NULL && window->bounds.x == 80);

    assert(umi_desktop_context_synchronizer_publish(
        synchronizer, "project-explorer", "project", "umicom-studio",
        "studio-context-1") == UMI_STATUS_OK);
    assert(umi_desktop_context_synchronizer_snapshot(
        synchronizer, &context) == UMI_STATUS_OK);
    assert(context.subscription_count == 5U);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_stop(bootstrap) == UMI_STATUS_OK);
    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}
