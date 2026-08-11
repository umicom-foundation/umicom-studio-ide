/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/helix_evidence_main.c
 *
 * PURPOSE:
 *   Record and retrieve one concise Helix evidence entry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This native command exercises the same Framework-backed service that future Studio GUI views will call.
 */

#include "umicom/studio/helix_evidence.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    UmiStudioAiPlatform *platform = NULL;
    const char *key = argc > 1 ? argv[1] : "batch";
    const char *value = argc > 2 ? argv[2] : "validation-passed";
    UmiStatus status = umi_studio_ai_platform_create(&platform);
    if (status == UMI_STATUS_OK) status = umi_studio_helix_record_evidence(platform, key, value);
    if (status == UMI_STATUS_OK) {
        printf("%s=%s\n", key, umi_studio_helix_evidence(platform, key));
    }
    umi_studio_ai_platform_destroy(platform);
    return status == UMI_STATUS_OK ? 0 : 1;
}
