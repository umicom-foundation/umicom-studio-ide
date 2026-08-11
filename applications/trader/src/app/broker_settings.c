/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/src/app/broker_settings.c
 *
 * PURPOSE:
 *   Build safe default simulation/paper broker connection settings.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file contains only Trader composition or product policy; reusable trading mechanics remain in Umicom Framework.
 */

#include <stdio.h>
#include "umicom/trader/broker_settings.h"

UmiIbkrConnectionSettings
umi_trader_default_ibkr_settings(UmiTradingEnvironment environment)
{
    UmiIbkrConnectionSettings settings = {0};

    (void)snprintf(settings.host,
                   sizeof(settings.host),
                   "%s",
                   "127.0.0.1");

    settings.port =
        environment == UMI_TRADING_LIVE ? 7496U : 7497U;
    settings.client_id = 17;
    settings.environment = environment;
    return settings;
}
