/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/src/app/watchlist.c
 *
 * PURPOSE:
 *   Compose Trader watchlist operations over Framework instruments.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file contains only Trader composition or product policy; reusable trading mechanics remain in Umicom Framework.
 */

#include "umicom/trader/watchlist.h"
UmiStatus umi_trader_watchlist_add(UmiTraderServices *s,const UmiInstrument *i){return s!=NULL?umi_watchlist_add(&s->watchlist,i):UMI_STATUS_INVALID_ARGUMENT;}
