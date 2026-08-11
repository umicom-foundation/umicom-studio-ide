/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/src/app/positions.c
 *
 * PURPOSE:
 *   Expose product-neutral position snapshots for future GTK/web chart panels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file contains only Trader composition or product policy; reusable trading mechanics remain in Umicom Framework.
 */

#include "umicom/trader/positions.h"
size_t umi_trader_position_count(const UmiTraderServices *s){return s!=NULL?s->positions.count:0U;}
const UmiPosition *umi_trader_position_at(const UmiTraderServices *s,size_t index){return s!=NULL&&index<s->positions.count?&s->positions.positions[index]:NULL;}
