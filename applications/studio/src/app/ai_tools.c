/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/ai_tools.c
 *
 * PURPOSE:
 *   Register a safe reference Studio tool and invoke it through policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The echo tool is intentionally harmless but exercises the same approval path later used by build, Git, file and Helix tools.
 */

#include "umicom/studio/ai_tools.h"

#include <stdio.h>

static UmiStatus echo_tool(const char *arguments_json,
                           char *output,
                           size_t output_capacity,
                           void *user_data)
{
    int written;
    (void)user_data;
    written = snprintf(output, output_capacity,
                       "echo:%s", arguments_json != NULL ? arguments_json : "");
    return written >= 0 && (size_t)written < output_capacity
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

UmiStatus umi_studio_ai_tools_register_defaults(UmiStudioAiPlatform *platform)
{
    UmiAiTool tool;
    UmiAiRuntime *runtime;
    UmiStatus status;
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    runtime = umi_studio_ai_platform_runtime(platform);
    if (runtime == NULL) return UMI_STATUS_INVALID_STATE;
    status = umi_ai_tool_init(&tool,
                              "studio.echo",
                              "Echo safe test arguments",
                              "studio.tools.echo",
                              echo_tool,
                              NULL);
    if (status != UMI_STATUS_OK) return status;
    return umi_ai_tool_registry_add(&runtime->tools, &tool);
}

UmiStatus umi_studio_ai_tools_echo(UmiStudioAiPlatform *platform,
                                   const char *arguments_json,
                                   int approved,
                                   char *output,
                                   size_t output_capacity)
{
    UmiAiRuntime *runtime;
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    runtime = umi_studio_ai_platform_runtime(platform);
    if (runtime == NULL) return UMI_STATUS_INVALID_STATE;
    return umi_ai_runtime_invoke_tool(runtime,
                                      "studio.echo",
                                      approved,
                                      arguments_json,
                                      output,
                                      output_capacity);
}
