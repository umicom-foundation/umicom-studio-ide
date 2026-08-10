/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/history_main.c
 *
 * PURPOSE:
 *   Provide a native Studio command for appending and inspecting persistent operational history records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/history.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioServices *services;
    UmiStatus status = umi_studio_bootstrap_create(&bootstrap);
    if (status != UMI_STATUS_OK) return 1;
    services = umi_studio_bootstrap_services(bootstrap);
    if (argc >= 4 && strcmp(argv[1], "append") == 0) {
        status = umi_studio_history_append(services,
                                           argv[2],
                                           UMI_STATUS_OK,
                                           argv[3],
                                           0U,
                                           NULL);
    } else {
        size_t index;
        size_t count = umi_studio_history_count(services);
        for (index = 0U; index < count; ++index) {
            UmiStudioHistoryRecord record;
            status = umi_studio_history_read(services, index, &record);
            if (status != UMI_STATUS_OK) break;
            (void)printf("%llu [%s] %s: %s\n",
                         (unsigned long long)record.sequence,
                         record.category,
                         umi_status_text(record.status),
                         record.message);
        }
        if (count == 0U) (void)printf("No Studio history records\n");
    }
    umi_studio_bootstrap_destroy(bootstrap);
    return status == UMI_STATUS_OK ? 0 : 1;
}
