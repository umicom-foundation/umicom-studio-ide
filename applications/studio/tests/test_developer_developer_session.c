/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_developer_session.c
 *
 * PURPOSE:
 *   Verify developer-session context ownership and monotonic revision handling.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A caller is allowed to pass a snapshot that contains any revision value.
 * The session must ignore that external revision and advance its own revision
 * exactly once for every successful update.
 */
#include <string.h>

#include "umicom/studio/developer_session.h"

int main(void)
{
    UmiStudioDeveloperSession *session = NULL;
    UmiStudioDeveloperSessionSnapshot initial;
    UmiStudioDeveloperSessionSnapshot context;
    UmiStudioDeveloperSessionSnapshot updated;

    if (umi_studio_developer_session_create(&session) != UMI_STATUS_OK) {
        return 1;
    }

    if (umi_studio_developer_session_snapshot(session, &initial) != UMI_STATUS_OK ||
        initial.revision == 0U) {
        umi_studio_developer_session_destroy(session);
        return 2;
    }

    memset(&context, 0, sizeof(context));
    context.struct_size = (uint32_t)sizeof(context);
    context.api_version = 1U;
    context.revision = 9999U; /* Must not control the session-owned revision. */
    (void)strcpy(context.project_id, "umicom-studio");
    (void)strcpy(context.configuration_id, "debug");
    (void)strcpy(context.active_file, "applications/studio/src/app/developer_session.c");

    if (umi_studio_developer_session_set_context(session, &context) != UMI_STATUS_OK) {
        umi_studio_developer_session_destroy(session);
        return 3;
    }

    if (umi_studio_developer_session_snapshot(session, &updated) != UMI_STATUS_OK) {
        umi_studio_developer_session_destroy(session);
        return 4;
    }

    if (updated.revision != initial.revision + 1U ||
        strcmp(updated.project_id, "umicom-studio") != 0 ||
        strcmp(updated.configuration_id, "debug") != 0) {
        umi_studio_developer_session_destroy(session);
        return 5;
    }

    context.revision = 0U;
    (void)strcpy(context.configuration_id, "release");
    if (umi_studio_developer_session_set_context(session, &context) != UMI_STATUS_OK ||
        umi_studio_developer_session_snapshot(session, &updated) != UMI_STATUS_OK) {
        umi_studio_developer_session_destroy(session);
        return 6;
    }

    if (updated.revision != initial.revision + 2U ||
        strcmp(updated.configuration_id, "release") != 0) {
        umi_studio_developer_session_destroy(session);
        return 7;
    }

    umi_studio_developer_session_destroy(session);
    return 0;
}
