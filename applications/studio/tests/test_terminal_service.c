/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_terminal_service.c
 *
 * PURPOSE:
 *   Verify Studio prepared-terminal ownership and session snapshots without executing a shell.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/terminal.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiStudioTerminalService *service = NULL;
    UmiStudioTerminalSnapshot snapshot;
    UmiClock clock = umi_clock_system();
    assert(umi_studio_terminal_service_create(".", &clock, &service) == UMI_STATUS_OK);
    assert(umi_studio_terminal_service_snapshot(service, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.session_count == 1U);
    assert(strcmp(snapshot.primary.session_id, "studio.primary") == 0);
    assert(umi_studio_terminal_service_manager(service) != NULL);
    assert(umi_studio_terminal_service_primary(service) != NULL);
    umi_studio_terminal_service_destroy(service);
    umi_clock_dispose(&clock);
    return 0;
}
