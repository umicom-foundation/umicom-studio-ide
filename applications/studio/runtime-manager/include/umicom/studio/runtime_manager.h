/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/runtime-manager/include/umicom/studio/runtime_manager.h
 *
 * PURPOSE:
 *   Define Studio's application catalogue and runtime-state controller over public Framework integration contracts.
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

#ifndef UMICOM_STUDIO_RUNTIME_MANAGER_H
#define UMICOM_STUDIO_RUNTIME_MANAGER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/integration/application.h"
#include "umicom/integration/presence.h"
#include "umicom/integration/registry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_RUNTIME_DESCRIPTION_CAPACITY 384U
#define UMI_STUDIO_RUNTIME_MAX_ENTRIES 64U

typedef enum UmiStudioRuntimeCategory {
    UMI_STUDIO_RUNTIME_CATEGORY_ALL = 0,
    UMI_STUDIO_RUNTIME_CATEGORY_DEVELOPMENT,
    UMI_STUDIO_RUNTIME_CATEGORY_FINANCE,
    UMI_STUDIO_RUNTIME_CATEGORY_AI,
    UMI_STUDIO_RUNTIME_CATEGORY_CREATOR,
    UMI_STUDIO_RUNTIME_CATEGORY_SYSTEM,
    UMI_STUDIO_RUNTIME_CATEGORY_SERVICE,
    UMI_STUDIO_RUNTIME_CATEGORY_COUNT
} UmiStudioRuntimeCategory;

typedef struct UmiStudioRuntimeEntry {
    UmiIntegrationApplication application;
    UmiStudioRuntimeCategory category;
    char description[UMI_STUDIO_RUNTIME_DESCRIPTION_CAPACITY];
    char resolved_executable[UMI_INTEGRATION_PATH_CAPACITY];
    bool installed;
    bool favourite;
} UmiStudioRuntimeEntry;

typedef struct UmiStudioRuntimeManager {
    UmiIntegrationRegistry registry;
    UmiIntegrationPresence presence;
    UmiStudioRuntimeEntry entries[UMI_STUDIO_RUNTIME_MAX_ENTRIES];
    size_t count;
} UmiStudioRuntimeManager;

void umi_studio_runtime_manager_init(UmiStudioRuntimeManager *manager);

UmiStatus umi_studio_runtime_manager_register(
    UmiStudioRuntimeManager *manager,
    const UmiIntegrationApplication *application,
    UmiStudioRuntimeCategory category,
    const char *description
);

UmiStudioRuntimeEntry *umi_studio_runtime_manager_find(
    UmiStudioRuntimeManager *manager,
    const char *application_id
);

const UmiStudioRuntimeEntry *umi_studio_runtime_manager_find_const(
    const UmiStudioRuntimeManager *manager,
    const char *application_id
);

UmiStatus umi_studio_runtime_manager_set_state(
    UmiStudioRuntimeManager *manager,
    const char *application_id,
    UmiIntegrationApplicationState state
);

UmiIntegrationApplicationState umi_studio_runtime_manager_state(
    const UmiStudioRuntimeManager *manager,
    const char *application_id
);

UmiStatus umi_studio_runtime_manager_set_executable(
    UmiStudioRuntimeManager *manager,
    const char *application_id,
    const char *executable_path
);

UmiStatus umi_studio_runtime_manager_set_favourite(
    UmiStudioRuntimeManager *manager,
    const char *application_id,
    bool favourite
);

UmiStatus umi_studio_runtime_manager_heartbeat(
    UmiStudioRuntimeManager *manager,
    const char *application_id,
    uint64_t now_ms,
    uint64_t lease_ms
);

bool umi_studio_runtime_manager_is_present(
    const UmiStudioRuntimeManager *manager,
    const char *application_id,
    uint64_t now_ms
);

size_t umi_studio_runtime_manager_count(
    const UmiStudioRuntimeManager *manager
);

const char *umi_studio_runtime_category_text(
    UmiStudioRuntimeCategory category
);

#ifdef __cplusplus
}
#endif

#endif
