/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/developer_session.c
 *
 * PURPOSE:
 *   Implement the active developer context with bounded value semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The session owns a copy of the active developer context.  Callers may supply
 * identifiers for the current project, build configuration, launch profile,
 * source-control repository, test item and active file, but callers do not own
 * the session revision counter.  The revision is incremented exactly once for
 * every successful context update so observers can reliably detect changes.
 */
#include "umicom/studio/developer_session.h"

#include <stdlib.h>
#include <string.h>

struct UmiStudioDeveloperSession {
    UmiStudioDeveloperSessionSnapshot state;
};

static void normalise(UmiStudioDeveloperSessionSnapshot *state)
{
    state->struct_size = (uint32_t)sizeof(*state);
    state->api_version = 1U;
    state->project_id[127U] = '\0';
    state->configuration_id[127U] = '\0';
    state->launch_profile_id[127U] = '\0';
    state->repository_id[127U] = '\0';
    state->test_item_id[127U] = '\0';
    state->active_file[511U] = '\0';
}

UmiStatus umi_studio_developer_session_create(UmiStudioDeveloperSession **out_session)
{
    UmiStudioDeveloperSession *session;

    if (out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_session = NULL;
    session = calloc(1U, sizeof(*session));
    if (session == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    session->state.revision = 1U;
    normalise(&session->state);
    *out_session = session;
    return UMI_STATUS_OK;
}

void umi_studio_developer_session_destroy(UmiStudioDeveloperSession *session)
{
    free(session);
}

UmiStatus umi_studio_developer_session_set_context(
    UmiStudioDeveloperSession *session,
    const UmiStudioDeveloperSessionSnapshot *context)
{
    uint64_t next_revision;

    if (session == NULL || context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Preserve the session-owned monotonic revision before copying caller
     * data.  Batch 26 copied context->revision first and then incremented it,
     * which allowed a caller-provided revision to reset or arbitrarily jump
     * the session revision.
     */
    next_revision = session->state.revision + 1U;
    session->state = *context;
    normalise(&session->state);
    session->state.revision = next_revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_developer_session_snapshot(
    const UmiStudioDeveloperSession *session,
    UmiStudioDeveloperSessionSnapshot *out_snapshot)
{
    if (session == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_snapshot = session->state;
    return UMI_STATUS_OK;
}
