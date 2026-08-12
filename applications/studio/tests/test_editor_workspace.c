/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_editor_workspace.c
 *
 * PURPOSE:
 *   Exercise Studio composition over the reusable Framework editor session.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The test uses no GTK objects. It verifies that Studio can compose and use
 * the same editor session model from console, GTK4 or future frontends.
 */
#include <string.h>
#include "umicom/studio/editor_workspace.h"

int main(void)
{
    UmiStudioEditorWorkspace *workspace = NULL;
    UmiStudioEditorWorkspaceSnapshot snapshot;
    UmiEditorDocumentSnapshot document = {0};

    if (umi_studio_editor_workspace_create(NULL, &workspace) != UMI_STATUS_OK) return 1;
    (void)strcpy(document.id, "readme");
    (void)strcpy(document.uri, "file:///README.md");
    (void)strcpy(document.language_id, "markdown");
    if (umi_editor_document_registry_upsert(
            umi_editor_session_document(umi_studio_editor_workspace_session(workspace)),
            &document) != UMI_STATUS_OK) return 2;
    if (umi_studio_editor_workspace_snapshot(workspace, &snapshot) != UMI_STATUS_OK) return 3;
    if (snapshot.editor.document_count != 1U) return 4;
    umi_studio_editor_workspace_destroy(workspace);
    return 0;
}
