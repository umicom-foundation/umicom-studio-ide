/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_ai_workspace.c
 *
 * PURPOSE:
 *   Verify the Studio AI Workspace reports live AuthorEngine integration state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/ai_workspace.h"

int main(void)
{
    UmiStudioServices *services = NULL;
    UmiStudioAiWorkspaceSnapshot snapshot;
    (void)umi_fs_remove_tree(".umicom");
    assert(umi_studio_services_create(NULL, NULL, &services) == UMI_STATUS_OK);
    assert(umi_studio_ai_workspace_snapshot(services, &snapshot)
        == UMI_STATUS_OK);
    assert(snapshot.available);
    assert(snapshot.runtime_count == 3U);
    assert(snapshot.context_source_count == 3U);
    assert(snapshot.session_count == 1U);
    assert(snapshot.item_count == 7U);
    assert(strcmp(snapshot.title, "AI Workspace") == 0);
    assert(strcmp(snapshot.active_session_id, "studio.session.default") == 0);
    umi_studio_services_destroy(services);
    assert(umi_fs_remove_tree(".umicom") == UMI_STATUS_OK);
    return 0;
}
