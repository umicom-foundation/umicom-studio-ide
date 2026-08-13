/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/build_workbench.c
 *
 * PURPOSE:
 *   Publish structured compiler/test diagnostics and raw output to the
 *   toolkit-neutral Studio workbench registries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/build_workbench.h"

#include <stdio.h>
#include <string.h>

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
}

UmiStatus umi_studio_build_publish_result(
    const UmiBuildResult *result,
    UmiUiProblemRegistry *problems,
    UmiUiOutputChannelRegistry *output_channels)
{
    UmiUiOutputChannelSnapshot channel;
    size_t index;
    if (result == NULL || problems == NULL || output_channels == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;

    memset(&channel, 0, sizeof(channel));
    (void)snprintf(channel.id, sizeof(channel.id), "build.%llu",
                   (unsigned long long)result->operation_id);
    (void)snprintf(channel.name, sizeof(channel.name), "Build %llu: %s",
                   (unsigned long long)result->operation_id,
                   umi_build_phase_text(result->phase));
    copy_text(channel.category, sizeof(channel.category), "build");
    copy_text(channel.text, sizeof(channel.text), result->output);
    channel.sequence = result->operation_id;
    channel.visible = 1;
    channel.preserve = 1;
    if (umi_ui_output_channel_registry_upsert(output_channels, &channel) !=
        UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;

    for (index = 0U; index < result->diagnostics.count; ++index) {
        const UmiBuildDiagnostic *diagnostic = &result->diagnostics.items[index];
        UmiUiProblemSnapshot problem;
        memset(&problem, 0, sizeof(problem));
        (void)snprintf(problem.id, sizeof(problem.id), "build.%llu.%zu",
                       (unsigned long long)result->operation_id, index);
        copy_text(problem.source, sizeof(problem.source), "Umicom Build");
        copy_text(problem.code, sizeof(problem.code), diagnostic->code);
        copy_text(problem.message, sizeof(problem.message), diagnostic->message);
        copy_text(problem.uri, sizeof(problem.uri), diagnostic->file);
        problem.line = (uint32_t)diagnostic->line;
        problem.column = (uint32_t)diagnostic->column;
        problem.severity = (int)diagnostic->severity;
        if (umi_ui_problem_registry_upsert(problems, &problem) != UMI_STATUS_OK)
            return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
