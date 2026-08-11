/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/src/cli/main.c
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
    UmiTraderApplication application = {0};
    if (umi_trader_application_start(&application) != UMI_STATUS_OK) {
        return 1;
    }

    char status[256];
    if (umi_trader_status_text(&application.services,
                               status,
                               sizeof(status)) == UMI_STATUS_OK) {
        puts(status);
    }

    umi_trader_application_stop(&application);
    return 0;
}
