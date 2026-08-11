/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/src/app/services.c
 *
 * PURPOSE:
 *   Own the shared Trader service container over Framework trading services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file contains only Trader composition or product policy; reusable trading mechanics remain in Umicom Framework.
 */

#include "umicom/trader/services.h"

UmiStatus umi_trader_services_init(UmiTraderServices *services)
{
    if (services == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *services = (UmiTraderServices){0};

    const UmiRiskLimit limits = {
        10.0,
        2000000.0,
        20.0,
        10000.0
    };

    umi_oms_init(&services->oms, limits);
    umi_watchlist_init(&services->watchlist);
    umi_research_stats_init(&services->research_stats);
    umi_position_book_init(&services->positions);

    const UmiStatus create_status =
        umi_simulated_broker_create(&services->simulated_broker_storage,
                                    &services->broker);
    if (create_status != UMI_STATUS_OK) {
        return create_status;
    }

    services->broker_created = 1;
    return services->broker.connect(services->broker.instance,
                                    UMI_TRADING_SIMULATION);
}

void umi_trader_services_destroy(UmiTraderServices *services)
{
    if (services != NULL && services->broker_created) {
        services->broker.destroy(services->broker.instance);
        services->broker_created = 0;
    }
}
