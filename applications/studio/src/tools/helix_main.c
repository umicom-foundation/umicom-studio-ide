/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/helix_main.c
 *
 * PURPOSE:
 *   Report the Studio Helix workflow and registered specialist agents.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This native command exercises the same Framework-backed service that future Studio GUI views will call.
 */

#include "umicom/studio/helix.h"
#include "umicom/studio/helix_agents.h"

#include <stdio.h>

int main(void)
{
    UmiStudioAiPlatform *platform = NULL;
    char output[512];
    size_t index;
    UmiStatus status = umi_studio_ai_platform_create(&platform);
    if (status == UMI_STATUS_OK) status = umi_studio_helix_status(platform, output, sizeof(output));
    if (status == UMI_STATUS_OK) {
        printf("%s\n", output);
        for (index = 0U; index < umi_studio_helix_agent_count(platform); ++index) {
            const UmiHelixAgent *agent = umi_studio_helix_agent_at(platform, index);
            printf("Agent: %s kind=%s permission=%s\n",
                   agent->agent_id,
                   umi_helix_agent_kind_text(agent->kind),
                   agent->permission);
        }
    }
    umi_studio_ai_platform_destroy(platform);
    return status == UMI_STATUS_OK ? 0 : 1;
}
