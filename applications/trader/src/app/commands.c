/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/src/app/commands.c
 *
 * PURPOSE:
 *   Publish the initial Trader command catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file contains only Trader composition or product policy; reusable trading mechanics remain in Umicom Framework.
 */

#include "umicom/trader/commands.h"
static const char *commands[]={"trader.market.watch","trader.order.submit","trader.order.cancel","trader.risk.report","trader.replay.start","trader.research.movement","trader.broker.status","trader.position.report"};
size_t umi_trader_command_count(void){return sizeof(commands)/sizeof(commands[0]);}
const char *umi_trader_command_at(size_t index){return index<umi_trader_command_count()?commands[index]:NULL;}
