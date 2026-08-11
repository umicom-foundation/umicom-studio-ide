/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/ai_main.c
 *
 * PURPOSE:
 *   Run a small Studio AI chat request against the shared reference provider.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This native command exercises the same Framework-backed service that future Studio GUI views will call.
 */

#include "umicom/studio/ai_chat.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    UmiStudioAiPlatform *platform = NULL;
    UmiAiResponse response;
    const char *prompt = argc > 1 ? argv[1] : "Introduce Umicom Framework.";
    UmiStatus status = umi_studio_ai_platform_create(&platform);
    if (status == UMI_STATUS_OK) status = umi_studio_ai_chat(platform, prompt, &response);
    if (status == UMI_STATUS_OK) {
        printf("Provider: %s\nModel: %s\nResponse: %s\nTokens: %llu\n",
               response.provider_id, response.model_id, response.text,
               (unsigned long long)response.usage.total_tokens);
    }
    umi_studio_ai_platform_destroy(platform);
    return status == UMI_STATUS_OK ? 0 : 1;
}
