/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/src/app/research.c
 *
 * PURPOSE:
 *   Track >=100-point movement research and factor observations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file contains only Trader composition or product policy; reusable trading mechanics remain in Umicom Framework.
 */

#include "umicom/trader/research.h"

void umi_trader_research_init(UmiTraderResearchLab *lab)
{
    if (lab == NULL) {
        return;
    }

    umi_research_stats_init(&lab->stats);
    umi_feature_vector_init(&lab->factors);
    lab->movement_threshold_points = 100.0;
    lab->movement_window_ms = 30LL * 60LL * 1000LL;
}

UmiMovementEvent
umi_trader_research_record_move(UmiTraderResearchLab *lab,
                                double start_price,
                                double end_price,
                                int64_t start_ms,
                                int64_t end_ms)
{
    if (lab == NULL) {
        return (UmiMovementEvent){0};
    }

    const UmiMovementEvent event =
        umi_movement_event_measure(start_price,
                                   end_price,
                                   start_ms,
                                   end_ms,
                                   lab->movement_threshold_points,
                                   lab->movement_window_ms);
    umi_research_stats_add(&lab->stats, &event);
    return event;
}
