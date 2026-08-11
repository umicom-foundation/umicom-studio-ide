/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/src/app/research_report.c
 *
 * PURPOSE:
 *   Render the movement-research summary used by the historical study workflow.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file contains only Trader composition or product policy; reusable trading mechanics remain in Umicom Framework.
 */

#include <stdio.h>
#include "umicom/trader/research_report.h"

UmiStatus umi_trader_research_report(const UmiTraderResearchLab *lab,
                                     char *buffer,
                                     size_t capacity)
{
    if (lab == NULL || buffer == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    const int written =
        snprintf(buffer,
                 capacity,
                 "observations=%llu qualifying=%llu "
                 "mean_points=%.2f threshold=%.2f window_ms=%lld",
                 (unsigned long long)lab->stats.observations,
                 (unsigned long long)lab->stats.qualifying_moves,
                 umi_research_stats_mean_points(&lab->stats),
                 lab->movement_threshold_points,
                 (long long)lab->movement_window_ms);

    return written >= 0 && (size_t)written < capacity
               ? UMI_STATUS_OK
               : UMI_STATUS_CAPACITY_EXCEEDED;
}
