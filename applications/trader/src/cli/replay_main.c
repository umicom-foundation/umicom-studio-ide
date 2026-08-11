/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/src/cli/replay_main.c
 *
 * PURPOSE:
 *   Provide a native headless Umicom Trader foundation command.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * These small executables prove the product can run without a GUI and exercise the same services future GTK and web frontends will use.
 */

#include <stdio.h>
#include "umicom/trader/trader.h"
int main(void){UmiTraderReplayLab lab;umi_trader_replay_lab_init(&lab,1U,0);UmiReplayEvent event={1U,1000,{0},{0}};(void)snprintf(event.type,sizeof(event.type),"%s","tick");return umi_trader_replay_lab_accept(&lab,&event)?0:1;}
