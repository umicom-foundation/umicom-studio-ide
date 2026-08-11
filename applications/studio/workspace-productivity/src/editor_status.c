/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/src/editor_status.c
 *
 * PURPOSE:
 *   Format source-editor cursor, selection, language, encoding and line-ending status.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is part of Studio workspace/editor productivity. The implementation
 * uses bounded C23 data structures and explicit ownership so the behaviour is
 * easy to inspect, test and later connect to the main Studio workbench.
 */

#include "umicom/studio/editor_status.h"

#include <stdio.h>
#include <string.h>

void umi_studio_editor_status_init(UmiStudioEditorStatus *status)
{
    if (status != NULL) {
        (void)memset(status, 0, sizeof(*status));
        status->line = 1U;
        status->column = 1U;
        (void)snprintf(status->language, sizeof(status->language), "Plain Text");
        (void)snprintf(status->encoding, sizeof(status->encoding), "UTF-8");
        status->line_ending = UMI_STUDIO_LINE_ENDING_LF;
    }
}

const char *umi_studio_line_ending_text(UmiStudioLineEnding line_ending)
{
    switch (line_ending) {
        case UMI_STUDIO_LINE_ENDING_LF: return "LF";
        case UMI_STUDIO_LINE_ENDING_CRLF: return "CRLF";
        case UMI_STUDIO_LINE_ENDING_CR: return "CR";
        default: return "?";
    }
}

UmiStatus umi_studio_editor_status_format(
    const UmiStudioEditorStatus *status,
    char *output,
    size_t output_capacity)
{
    int written;

    if (status == NULL || output == NULL || output_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(
        output,
        output_capacity,
        "Ln %zu, Col %zu  |  %s  |  %s  |  %s%s",
        status->line,
        status->column,
        status->language,
        status->encoding,
        umi_studio_line_ending_text(status->line_ending),
        status->selection_length > 0U ? "  |  Selection" : ""
    );

    return written >= 0 && (size_t)written < output_capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}
