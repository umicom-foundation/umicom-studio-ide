/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/runtime-manager/src/runtime_manager.c
 *
 * PURPOSE:
 *   Implement Studio application registration, state, presence, favourites and executable metadata.
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

#include "umicom/studio/runtime_manager.h"

#include <stdio.h>
#include <string.h>

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    int written;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(destination, capacity, "%s", source);
    if (written < 0 || (size_t)written >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    return UMI_STATUS_OK;
}

void umi_studio_runtime_manager_init(UmiStudioRuntimeManager *manager)
{
    if (manager == NULL) {
        return;
    }

    (void)memset(manager, 0, sizeof(*manager));
    umi_integration_registry_init(&manager->registry);
    umi_integration_presence_init(&manager->presence);
}

UmiStatus umi_studio_runtime_manager_register(
    UmiStudioRuntimeManager *manager,
    const UmiIntegrationApplication *application,
    UmiStudioRuntimeCategory category,
    const char *description)
{
    UmiStatus status;
    UmiStudioRuntimeEntry *entry;

    if (manager == NULL || application == NULL ||
        application->id[0] == '\0' ||
        category <= UMI_STUDIO_RUNTIME_CATEGORY_ALL ||
        category >= UMI_STUDIO_RUNTIME_CATEGORY_COUNT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (manager->count >= UMI_STUDIO_RUNTIME_MAX_ENTRIES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_integration_registry_register(&manager->registry, application);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    entry = &manager->entries[manager->count];
    (void)memset(entry, 0, sizeof(*entry));
    entry->application = *application;
    entry->category = category;

    status = copy_text(entry->description,
                       sizeof(entry->description),
                       description != NULL ? description : "");
    if (status != UMI_STATUS_OK) {
        (void)umi_integration_registry_unregister(&manager->registry,
                                                  application->id);
        return status;
    }

    if (application->executable[0] != '\0') {
        status = copy_text(entry->resolved_executable,
                           sizeof(entry->resolved_executable),
                           application->executable);
        if (status != UMI_STATUS_OK) {
            (void)umi_integration_registry_unregister(&manager->registry,
                                                      application->id);
            return status;
        }
    }

    ++manager->count;
    return UMI_STATUS_OK;
}

UmiStudioRuntimeEntry *umi_studio_runtime_manager_find(
    UmiStudioRuntimeManager *manager,
    const char *application_id)
{
    size_t index;

    if (manager == NULL || application_id == NULL) {
        return NULL;
    }

    for (index = 0U; index < manager->count; ++index) {
        if (strcmp(manager->entries[index].application.id,
                   application_id) == 0) {
            return &manager->entries[index];
        }
    }

    return NULL;
}

const UmiStudioRuntimeEntry *umi_studio_runtime_manager_find_const(
    const UmiStudioRuntimeManager *manager,
    const char *application_id)
{
    size_t index;

    if (manager == NULL || application_id == NULL) {
        return NULL;
    }

    for (index = 0U; index < manager->count; ++index) {
        if (strcmp(manager->entries[index].application.id,
                   application_id) == 0) {
            return &manager->entries[index];
        }
    }

    return NULL;
}

UmiStatus umi_studio_runtime_manager_set_state(
    UmiStudioRuntimeManager *manager,
    const char *application_id,
    UmiIntegrationApplicationState state)
{
    if (manager == NULL || application_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return umi_integration_registry_set_state(&manager->registry,
                                              application_id,
                                              state);
}

UmiIntegrationApplicationState umi_studio_runtime_manager_state(
    const UmiStudioRuntimeManager *manager,
    const char *application_id)
{
    const UmiIntegrationRegistryEntry *entry;

    if (manager == NULL || application_id == NULL) {
        return UMI_INTEGRATION_APP_UNKNOWN;
    }

    entry = umi_integration_registry_find_const(&manager->registry,
                                                application_id);
    return entry != NULL ? entry->state : UMI_INTEGRATION_APP_UNKNOWN;
}

UmiStatus umi_studio_runtime_manager_set_executable(
    UmiStudioRuntimeManager *manager,
    const char *application_id,
    const char *executable_path)
{
    UmiStudioRuntimeEntry *entry;
    UmiStatus status;

    if (manager == NULL || application_id == NULL ||
        executable_path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    entry = umi_studio_runtime_manager_find(manager, application_id);
    if (entry == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    status = copy_text(entry->resolved_executable,
                       sizeof(entry->resolved_executable),
                       executable_path);
    if (status == UMI_STATUS_OK) {
        entry->installed = executable_path[0] != '\0';
        entry->application.enabled = entry->installed;
        (void)umi_studio_runtime_manager_set_state(
            manager,
            application_id,
            entry->installed
                ? UMI_INTEGRATION_APP_AVAILABLE
                : UMI_INTEGRATION_APP_DISCOVERED
        );
    }

    return status;
}

UmiStatus umi_studio_runtime_manager_set_favourite(
    UmiStudioRuntimeManager *manager,
    const char *application_id,
    bool favourite)
{
    UmiStudioRuntimeEntry *entry;

    if (manager == NULL || application_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    entry = umi_studio_runtime_manager_find(manager, application_id);
    if (entry == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    entry->favourite = favourite;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_runtime_manager_heartbeat(
    UmiStudioRuntimeManager *manager,
    const char *application_id,
    uint64_t now_ms,
    uint64_t lease_ms)
{
    UmiStatus status;

    if (manager == NULL || application_id == NULL || lease_ms == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (umi_studio_runtime_manager_find(manager, application_id) == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    status = umi_integration_presence_heartbeat(&manager->presence,
                                                application_id,
                                                now_ms,
                                                lease_ms);
    if (status == UMI_STATUS_OK) {
        status = umi_studio_runtime_manager_set_state(
            manager,
            application_id,
            UMI_INTEGRATION_APP_RUNNING
        );
    }

    return status;
}

bool umi_studio_runtime_manager_is_present(
    const UmiStudioRuntimeManager *manager,
    const char *application_id,
    uint64_t now_ms)
{
    if (manager == NULL || application_id == NULL) {
        return false;
    }

    return umi_integration_presence_is_active(&manager->presence,
                                              application_id,
                                              now_ms);
}

size_t umi_studio_runtime_manager_count(
    const UmiStudioRuntimeManager *manager)
{
    return manager != NULL ? manager->count : 0U;
}

const char *umi_studio_runtime_category_text(
    UmiStudioRuntimeCategory category)
{
    switch (category) {
        case UMI_STUDIO_RUNTIME_CATEGORY_ALL: return "All";
        case UMI_STUDIO_RUNTIME_CATEGORY_DEVELOPMENT: return "Development";
        case UMI_STUDIO_RUNTIME_CATEGORY_FINANCE: return "Finance";
        case UMI_STUDIO_RUNTIME_CATEGORY_AI: return "AI";
        case UMI_STUDIO_RUNTIME_CATEGORY_CREATOR: return "Creator";
        case UMI_STUDIO_RUNTIME_CATEGORY_SYSTEM: return "System";
        case UMI_STUDIO_RUNTIME_CATEGORY_SERVICE: return "Service";
        default: return "Unknown";
    }
}
