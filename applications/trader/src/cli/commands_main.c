/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/src/cli/commands_main.c
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
int main(void){for(size_t i=0U;i<umi_trader_command_count();i++)puts(umi_trader_command_at(i));return 0;}
