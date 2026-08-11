/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/ai_chat.c
 *
 * PURPOSE:
 *   Dispatch Studio chat prompts through the shared AI runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * All provider selection and policy checks happen inside the Framework runtime, keeping Studio chat code deliberately thin.
 */

#include "umicom/studio/ai_chat.h"

#include <stdio.h>

UmiStatus umi_studio_ai_chat(UmiStudioAiPlatform *platform,
                             const char *prompt,
                             UmiAiResponse *response)
{
    UmiAiRequest request;
    UmiAiMessage message;
    UmiAiRuntime *runtime;
    const char *provider;
    UmiStatus status;

    if (platform == NULL || prompt == NULL || response == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    runtime = umi_studio_ai_platform_runtime(platform);
    provider = umi_studio_ai_platform_default_provider(platform);
    if (runtime == NULL || provider == NULL) return UMI_STATUS_INVALID_STATE;

    umi_ai_request_init(&request);
    (void)snprintf(request.model_id, sizeof(request.model_id),
                   "%s", "studio-reference");
    status = umi_ai_message_set(&message, UMI_AI_ROLE_USER, "user", prompt);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_request_add_message(&request, &message);
    if (status != UMI_STATUS_OK) return status;
    return umi_ai_runtime_generate(runtime, provider, 1, &request, response);
}
