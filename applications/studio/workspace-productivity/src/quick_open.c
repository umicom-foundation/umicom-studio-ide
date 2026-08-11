/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/src/quick_open.c
 *
 * PURPOSE:
 *   Implement fuzzy subsequence scoring and deterministic ranked quick-open results.
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

#include "umicom/studio/quick_open.h"

#include <ctype.h>
#include <string.h>

int umi_studio_quick_open_score(const char *query, const char *candidate)
{
    size_t query_index = 0U;
    size_t candidate_index;
    int score = 0;
    int contiguous = 0;

    if (query == NULL || candidate == NULL) {
        return -1;
    }

    if (query[0] == '\0') {
        return 1;
    }

    for (candidate_index = 0U;
         candidate[candidate_index] != '\0' && query[query_index] != '\0';
         ++candidate_index) {
        const unsigned char query_char =
            (unsigned char)tolower((unsigned char)query[query_index]);
        const unsigned char candidate_char =
            (unsigned char)tolower((unsigned char)candidate[candidate_index]);

        if (query_char == candidate_char) {
            score += 10;
            if (candidate_index == 0U) {
                score += 8;
            }
            if (contiguous != 0) {
                score += 5;
            }
            contiguous = 1;
            ++query_index;
        } else {
            contiguous = 0;
            score -= 1;
        }
    }

    if (query[query_index] != '\0') {
        return -1;
    }

    if (strstr(candidate, query) != NULL) {
        score += 25;
    }

    return score;
}

static void insert_result(UmiStudioQuickOpenResults *results,
                          const UmiStudioQuickOpenCandidate *candidate)
{
    size_t position;
    size_t move_count;

    if (candidate->score < 0) {
        return;
    }

    position = 0U;
    while (position < results->count &&
           results->items[position].score >= candidate->score) {
        ++position;
    }

    if (position >= UMI_STUDIO_QUICK_OPEN_MAX_RESULTS) {
        return;
    }

    if (results->count < UMI_STUDIO_QUICK_OPEN_MAX_RESULTS) {
        ++results->count;
    }

    move_count = results->count - position - 1U;
    if (move_count > 0U) {
        (void)memmove(&results->items[position + 1U],
                      &results->items[position],
                      move_count * sizeof(results->items[0]));
    }

    results->items[position] = *candidate;
}

UmiStatus umi_studio_quick_open_search(
    const char *query,
    const UmiStudioQuickOpenCandidate *candidates,
    size_t candidate_count,
    UmiStudioQuickOpenResults *results)
{
    size_t index;

    if (query == NULL || candidates == NULL || results == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (candidate_count > UMI_STUDIO_QUICK_OPEN_MAX_CANDIDATES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memset(results, 0, sizeof(*results));

    for (index = 0U; index < candidate_count; ++index) {
        UmiStudioQuickOpenCandidate scored = candidates[index];
        int label_score = umi_studio_quick_open_score(query, scored.label);
        int detail_score = umi_studio_quick_open_score(query, scored.detail);

        scored.score = label_score > detail_score ? label_score : detail_score;
        insert_result(results, &scored);
    }

    return UMI_STATUS_OK;
}
