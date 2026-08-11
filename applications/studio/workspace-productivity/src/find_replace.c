/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/src/find_replace.c
 *
 * PURPOSE:
 *   Implement bounded literal find/replace with case and whole-word options.
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

#include "umicom/studio/find_replace.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static bool is_word_character(char c)
{
    const unsigned char value = (unsigned char)c;
    return isalnum(value) != 0 || c == '_';
}

static bool text_equal_n(const char *left,
                         const char *right,
                         size_t length,
                         bool case_sensitive)
{
    size_t index;

    for (index = 0U; index < length; ++index) {
        unsigned char a = (unsigned char)left[index];
        unsigned char b = (unsigned char)right[index];

        if (!case_sensitive) {
            a = (unsigned char)tolower(a);
            b = (unsigned char)tolower(b);
        }

        if (a != b) {
            return false;
        }
    }

    return true;
}

static bool occurrence_matches(const char *text,
                               size_t text_length,
                               size_t offset,
                               const UmiStudioFindReplace *operation)
{
    const size_t search_length = strlen(operation->search);

    if (offset + search_length > text_length) {
        return false;
    }

    if (!text_equal_n(text + offset,
                      operation->search,
                      search_length,
                      operation->case_sensitive)) {
        return false;
    }

    if (operation->whole_word) {
        if (offset > 0U && is_word_character(text[offset - 1U])) {
            return false;
        }

        if (offset + search_length < text_length &&
            is_word_character(text[offset + search_length])) {
            return false;
        }
    }

    return true;
}

void umi_studio_find_replace_init(UmiStudioFindReplace *operation)
{
    if (operation != NULL) {
        (void)memset(operation, 0, sizeof(*operation));
    }
}

UmiStatus umi_studio_find_replace_set(
    UmiStudioFindReplace *operation,
    const char *search,
    const char *replacement)
{
    int written;

    if (operation == NULL || search == NULL || replacement == NULL ||
        search[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(operation->search,
                       sizeof(operation->search),
                       "%s",
                       search);
    if (written < 0 || (size_t)written >= sizeof(operation->search)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    written = snprintf(operation->replacement,
                       sizeof(operation->replacement),
                       "%s",
                       replacement);
    return written >= 0 &&
           (size_t)written < sizeof(operation->replacement)
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

UmiStatus umi_studio_find_count(
    const char *text,
    const UmiStudioFindReplace *operation,
    size_t *count)
{
    const size_t text_length = text != NULL ? strlen(text) : 0U;
    const size_t search_length =
        operation != NULL ? strlen(operation->search) : 0U;
    size_t offset = 0U;

    if (text == NULL || operation == NULL || count == NULL ||
        search_length == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (operation->regular_expression) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    *count = 0U;

    while (offset + search_length <= text_length) {
        if (occurrence_matches(text, text_length, offset, operation)) {
            ++(*count);
            offset += search_length;
        } else {
            ++offset;
        }
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_studio_replace_all(
    const char *text,
    const UmiStudioFindReplace *operation,
    char *output,
    size_t output_capacity,
    size_t *replacement_count)
{
    const size_t text_length = text != NULL ? strlen(text) : 0U;
    const size_t search_length =
        operation != NULL ? strlen(operation->search) : 0U;
    const size_t replacement_length =
        operation != NULL ? strlen(operation->replacement) : 0U;
    size_t source = 0U;
    size_t destination = 0U;

    if (text == NULL || operation == NULL || output == NULL ||
        output_capacity == 0U || replacement_count == NULL ||
        search_length == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (operation->regular_expression) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    *replacement_count = 0U;

    while (source < text_length) {
        if (occurrence_matches(text, text_length, source, operation)) {
            if (destination + replacement_length >= output_capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            (void)memcpy(output + destination,
                         operation->replacement,
                         replacement_length);
            destination += replacement_length;
            source += search_length;
            ++(*replacement_count);
        } else {
            if (destination + 1U >= output_capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            output[destination++] = text[source++];
        }
    }

    output[destination] = '\0';
    return UMI_STATUS_OK;
}
