/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/helix_agent_operations.c
 * PURPOSE: Implement canonical Studio Helix v2 action descriptors.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/helix_agent_operations.h"

static UmiStatus make_action(const char *id, UmiHelixActionKind kind,
                             const char *summary, const char *target,
                             UmiHelixAction *out_action)
{
    return umi_helix_action_init(
        out_action, id, kind, summary, target, 3U);
}

UmiStatus umi_studio_helix_agent_filesystem_action(
    const char *target, UmiHelixAction *out_action)
{
    return make_action("studio.helix.filesystem", UMI_HELIX_ACTION_FILESYSTEM,
                       "Apply approved candidate files", target, out_action);
}

UmiStatus umi_studio_helix_agent_build_action(
    const char *target, UmiHelixAction *out_action)
{
    return make_action("studio.helix.build", UMI_HELIX_ACTION_BUILD,
                       "Build isolated candidate", target, out_action);
}

UmiStatus umi_studio_helix_agent_test_action(
    const char *target, UmiHelixAction *out_action)
{
    return make_action("studio.helix.test", UMI_HELIX_ACTION_TEST,
                       "Run focused candidate tests", target, out_action);
}

UmiStatus umi_studio_helix_agent_review_action(
    const char *target, UmiHelixAction *out_action)
{
    return make_action("studio.helix.review", UMI_HELIX_ACTION_REVIEW,
                       "Review candidate evidence", target, out_action);
}

UmiStatus umi_studio_helix_agent_source_control_action(
    const char *target, UmiHelixAction *out_action)
{
    return make_action("studio.helix.source-control",
                       UMI_HELIX_ACTION_SOURCE_CONTROL,
                       "Apply approved source-control operation", target,
                       out_action);
}
