/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/runtime-manager/src/runtime_snapshot.c
 *
 * PURPOSE:
 *   Build summary counts used by the Application Hub dashboard.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is part of the Studio Application Hub / Runtime Manager.
 * The code is intentionally explicit and bounded so a new C developer can
 * follow the data flow without hidden ownership or private product coupling.
 */

#include "umicom/studio/runtime_snapshot.h"

#include <string.h>

UmiStatus umi_studio_runtime_snapshot_build(
    const UmiStudioRuntimeManager *manager,
    UmiStudioRuntimeSnapshot *snapshot)
{
    size_t index;

    if (manager == NULL || snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(snapshot, 0, sizeof(*snapshot));
    snapshot->total = manager->count;

    for (index = 0U; index < manager->count; ++index) {
        const UmiStudioRuntimeEntry *entry = &manager->entries[index];
        UmiIntegrationApplicationState state =
            umi_studio_runtime_manager_state(manager, entry->application.id);

        if (entry->installed) {
            ++snapshot->installed;
        }
        if (entry->favourite) {
            ++snapshot->favourites;
        }

        if (entry->category > UMI_STUDIO_RUNTIME_CATEGORY_ALL &&
            entry->category < UMI_STUDIO_RUNTIME_CATEGORY_COUNT) {
            ++snapshot->by_category[entry->category];
        }

        switch (state) {
            case UMI_INTEGRATION_APP_RUNNING:
                ++snapshot->running;
                break;
            case UMI_INTEGRATION_APP_STOPPED:
                ++snapshot->stopped;
                break;
            case UMI_INTEGRATION_APP_FAILED:
                ++snapshot->failed;
                break;
            default:
                break;
        }
    }

    return UMI_STATUS_OK;
}
