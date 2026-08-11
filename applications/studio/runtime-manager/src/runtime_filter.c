/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/runtime-manager/src/runtime_filter.c
 *
 * PURPOSE:
 *   Implement Application Hub free-text, category, favourite, running and installed filters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is part of the Studio Application Hub / Runtime Manager.
 * The code is intentionally explicit and bounded so a new C developer can
 * follow the data flow without hidden ownership or private product coupling.
 */

#include "umicom/studio/runtime_filter.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static bool contains_case_insensitive(const char *haystack,
                                      const char *needle)
{
    size_t haystack_length;
    size_t needle_length;
    size_t start;
    size_t offset;

    if (haystack == NULL || needle == NULL) {
        return false;
    }

    needle_length = strlen(needle);
    if (needle_length == 0U) {
        return true;
    }

    haystack_length = strlen(haystack);
    if (needle_length > haystack_length) {
        return false;
    }

    for (start = 0U; start + needle_length <= haystack_length; ++start) {
        bool equal = true;
        for (offset = 0U; offset < needle_length; ++offset) {
            if (tolower((unsigned char)haystack[start + offset]) !=
                tolower((unsigned char)needle[offset])) {
                equal = false;
                break;
            }
        }
        if (equal) {
            return true;
        }
    }

    return false;
}

void umi_studio_runtime_filter_init(UmiStudioRuntimeFilter *filter)
{
    if (filter == NULL) {
        return;
    }

    (void)memset(filter, 0, sizeof(*filter));
    filter->category = UMI_STUDIO_RUNTIME_CATEGORY_ALL;
}

bool umi_studio_runtime_filter_match(
    const UmiStudioRuntimeFilter *filter,
    const UmiStudioRuntimeEntry *entry,
    UmiIntegrationApplicationState state)
{
    size_t index;
    bool query_match;

    if (filter == NULL || entry == NULL) {
        return false;
    }

    if (filter->category != UMI_STUDIO_RUNTIME_CATEGORY_ALL &&
        filter->category != entry->category) {
        return false;
    }

    if (filter->favourites_only && !entry->favourite) {
        return false;
    }

    if (filter->running_only && state != UMI_INTEGRATION_APP_RUNNING) {
        return false;
    }

    if (filter->installed_only && !entry->installed) {
        return false;
    }

    if (filter->query[0] == '\0') {
        return true;
    }

    query_match =
        contains_case_insensitive(entry->application.id, filter->query) ||
        contains_case_insensitive(entry->application.name, filter->query) ||
        contains_case_insensitive(entry->description, filter->query) ||
        contains_case_insensitive(umi_studio_runtime_category_text(entry->category),
                                  filter->query);

    if (query_match) {
        return true;
    }

    for (index = 0U; index < entry->application.capability_count; ++index) {
        if (contains_case_insensitive(entry->application.capabilities[index],
                                      filter->query)) {
            return true;
        }
    }

    return false;
}
