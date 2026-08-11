/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/src/session_state.c
 *
 * PURPOSE:
 *   Serialize and restore a stable text subset of workspace/document session state.
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

#include "umicom/studio/session_state.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void umi_studio_session_state_init(UmiStudioSessionState *state)
{
    if (state != NULL) {
        (void)memset(state, 0, sizeof(*state));
        umi_studio_document_set_init(&state->documents);
    }
}

UmiStatus umi_studio_session_state_set_workspace(
    UmiStudioSessionState *state,
    const char *workspace_path)
{
    int written;

    if (state == NULL || workspace_path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(state->workspace_path,
                       sizeof(state->workspace_path),
                       "%s",
                       workspace_path);
    return written >= 0 && (size_t)written < sizeof(state->workspace_path)
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

UmiStatus umi_studio_session_state_encode(
    const UmiStudioSessionState *state,
    char *output,
    size_t output_capacity)
{
    size_t used = 0U;
    size_t index;
    int written;

    if (state == NULL || output == NULL || output_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(output,
                       output_capacity,
                       "workspace=%s\n",
                       state->workspace_path);
    if (written < 0 || (size_t)written >= output_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    used = (size_t)written;

    for (index = 0U; index < state->documents.count; ++index) {
        const UmiStudioWorkspaceDocument *document =
            &state->documents.documents[index];

        written = snprintf(
            output + used,
            output_capacity - used,
            "doc|%s|%s|%zu|%zu|%u|%u|%u|%u\n",
            document->path,
            document->language,
            document->line,
            document->column,
            document->dirty ? 1U : 0U,
            document->pinned ? 1U : 0U,
            document->preview ? 1U : 0U,
            document->active ? 1U : 0U
        );

        if (written < 0 || (size_t)written >= output_capacity - used) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        used += (size_t)written;
    }

    return UMI_STATUS_OK;
}

static unsigned parse_unsigned(const char *text)
{
    return text != NULL ? (unsigned)strtoul(text, NULL, 10) : 0U;
}

static UmiStatus parse_document_line(char *line, UmiStudioSessionState *state)
{
    char *fields[9];
    size_t count = 0U;
    char *cursor = line;

    while (count < 9U) {
        fields[count++] = cursor;
        cursor = strchr(cursor, '|');
        if (cursor == NULL) {
            break;
        }
        *cursor = '\0';
        ++cursor;
    }

    if (count != 9U || strcmp(fields[0], "doc") != 0) {
        return UMI_STATUS_PARSE_ERROR;
    }

    {
        UmiStatus status = umi_studio_document_set_open(
            &state->documents,
            fields[1],
            fields[2],
            parse_unsigned(fields[7]) != 0U
        );
        UmiStudioWorkspaceDocument *document;

        if (status != UMI_STATUS_OK) {
            return status;
        }

        document = umi_studio_document_set_find(&state->documents, fields[1]);
        if (document == NULL) {
            return UMI_STATUS_INTERNAL_ERROR;
        }

        document->line = (size_t)strtoull(fields[3], NULL, 10);
        document->column = (size_t)strtoull(fields[4], NULL, 10);
        document->dirty = parse_unsigned(fields[5]) != 0U;
        document->pinned = parse_unsigned(fields[6]) != 0U;
        document->preview = parse_unsigned(fields[7]) != 0U;
        document->active = parse_unsigned(fields[8]) != 0U;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_studio_session_state_decode(
    const char *text,
    UmiStudioSessionState *state)
{
    char buffer[UMI_STUDIO_SESSION_TEXT_CAPACITY];
    char *cursor;

    if (text == NULL || state == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (strlen(text) >= sizeof(buffer)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)snprintf(buffer, sizeof(buffer), "%s", text);
    umi_studio_session_state_init(state);

    cursor = buffer;
    while (*cursor != '\0') {
        char *newline = strchr(cursor, '\n');
        UmiStatus status = UMI_STATUS_OK;

        if (newline != NULL) {
            *newline = '\0';
        }

        if (strncmp(cursor, "workspace=", 10U) == 0) {
            status = umi_studio_session_state_set_workspace(state,
                                                            cursor + 10U);
        } else if (strncmp(cursor, "doc|", 4U) == 0) {
            status = parse_document_line(cursor, state);
        } else if (cursor[0] != '\0') {
            status = UMI_STATUS_PARSE_ERROR;
        }

        if (status != UMI_STATUS_OK) {
            return status;
        }

        if (newline == NULL) {
            break;
        }
        cursor = newline + 1U;
    }

    return UMI_STATUS_OK;
}
