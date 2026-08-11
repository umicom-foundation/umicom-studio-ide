/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/ai_platform.c
 *
 * PURPOSE:
 *   Create Studio's shared AI and Helix composition over Framework services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The built-in reference provider is deterministic and test-safe; real local and remote providers plug into the same Framework registry.
 */

#include "umicom/studio/ai_platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiStudioAiPlatform {
    UmiAiRuntime ai;
    UmiHelixRuntime helix;
    char default_provider[UMI_AI_ID_CAPACITY];
};

static UmiStatus studio_local_generate(void *instance,
                                       const UmiAiRequest *request,
                                       UmiAiResponse *response)
{
    const UmiAiMessage *last;
    int written;
    (void)instance;
    if (request == NULL || response == NULL || request->message_count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    last = &request->messages[request->message_count - 1U];
    umi_ai_response_init(response);
    written = snprintf(response->provider_id, sizeof(response->provider_id),
                       "%s", "studio.local");
    if (written < 0 || (size_t)written >= sizeof(response->provider_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    written = snprintf(response->model_id, sizeof(response->model_id),
                       "%s", request->model_id[0] != '\0'
                           ? request->model_id : "studio-reference");
    if (written < 0 || (size_t)written >= sizeof(response->model_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    written = snprintf(response->text, sizeof(response->text),
                       "Studio local reference provider received: %s", last->text);
    if (written < 0 || (size_t)written >= sizeof(response->text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    response->finish_reason = UMI_AI_FINISH_STOP;
    response->usage.input_tokens = umi_ai_estimate_tokens(last->text);
    response->usage.output_tokens = umi_ai_estimate_tokens(response->text);
    response->usage.total_tokens =
        response->usage.input_tokens + response->usage.output_tokens;
    return UMI_STATUS_OK;
}

static UmiStatus studio_local_health(void *instance, UmiAiHealth *health)
{
    (void)instance;
    if (health == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_ai_health_init(health);
    (void)snprintf(health->provider_id, sizeof(health->provider_id),
                   "%s", "studio.local");
    health->available = 1;
    (void)snprintf(health->message, sizeof(health->message),
                   "%s", "Reference provider is available");
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_ai_platform_create(UmiStudioAiPlatform **out_platform)
{
    UmiStudioAiPlatform *platform;
    UmiAiProviderV1 provider;
    UmiStatus status;

    if (out_platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_platform = NULL;
    platform = (UmiStudioAiPlatform *)calloc(1U, sizeof(*platform));
    if (platform == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    umi_ai_runtime_init(&platform->ai);
    platform->ai.policy.allow_tools = 1;
    platform->ai.policy.require_tool_approval = 1;
    umi_helix_runtime_init(&platform->helix, "studio.helix");
    status = umi_helix_runtime_register_default_agents(&platform->helix,
                                                       "studio.local");
    if (status != UMI_STATUS_OK) {
        free(platform);
        return status;
    }

    (void)memset(&provider, 0, sizeof(provider));
    provider.structure_size = (uint32_t)sizeof(provider);
    provider.abi_version = 1U;
    provider.provider_id = "studio.local";
    provider.kind = UMI_AI_PROVIDER_TEST;
    provider.generate = studio_local_generate;
    provider.health = studio_local_health;

    status = umi_ai_provider_registry_add(&platform->ai.providers, &provider);
    if (status != UMI_STATUS_OK) {
        free(platform);
        return status;
    }
    (void)snprintf(platform->default_provider,
                   sizeof(platform->default_provider),
                   "%s", provider.provider_id);
    *out_platform = platform;
    return UMI_STATUS_OK;
}

void umi_studio_ai_platform_destroy(UmiStudioAiPlatform *platform)
{
    free(platform);
}

UmiAiRuntime *umi_studio_ai_platform_runtime(UmiStudioAiPlatform *platform)
{
    return platform != NULL ? &platform->ai : NULL;
}

UmiHelixRuntime *umi_studio_ai_platform_helix(UmiStudioAiPlatform *platform)
{
    return platform != NULL ? &platform->helix : NULL;
}

const char *umi_studio_ai_platform_default_provider(
    const UmiStudioAiPlatform *platform)
{
    return platform != NULL ? platform->default_provider : NULL;
}
