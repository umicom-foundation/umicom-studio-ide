/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/src/app/status.c
 *
 * PURPOSE:
 *   Render a concise text status for CLI and future workbench adapters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file contains only Trader composition or product policy; reusable trading mechanics remain in Umicom Framework.
 */

#include <stdio.h>
#include "umicom/trader/status.h"

UmiStatus umi_trader_status_text(const UmiTraderServices *services,
                                 char *buffer,
                                 size_t capacity)
{
    if (services == NULL || buffer == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    const int written =
        snprintf(buffer,
                 capacity,
                 "watchlist=%zu orders=%zu positions=%zu research=%llu",
                 services->watchlist.count,
                 services->oms.orders.count,
                 services->positions.count,
                 (unsigned long long)services->research_stats.observations);

    return written >= 0 && (size_t)written < capacity
               ? UMI_STATUS_OK
               : UMI_STATUS_CAPACITY_EXCEEDED;
}
