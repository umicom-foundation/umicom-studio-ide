/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/src/cli/research_main.c
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

int main(void)
{
    UmiTraderResearchLab lab;
    umi_trader_research_init(&lab);

    (void)umi_trader_research_record_move(&lab,
                                          30000.0,
                                          30125.0,
                                          0,
                                          1800000);

    char report[256];
    if (umi_trader_research_report(&lab,
                                   report,
                                   sizeof(report)) != UMI_STATUS_OK) {
        return 1;
    }

    puts(report);
    return lab.stats.qualifying_moves == 1U ? 0 : 2;
}
