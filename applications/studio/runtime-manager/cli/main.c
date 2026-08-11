/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/runtime-manager/cli/main.c
 *
 * PURPOSE:
 *   Provide a no-GTK CLI for inspecting the same Application Hub runtime model.
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

#include <stdio.h>
#include <string.h>

#include "umicom/studio/runtime_catalogue.h"
#include "umicom/studio/runtime_launch_plan.h"
#include "umicom/studio/runtime_snapshot.h"

static void print_entry(const UmiStudioRuntimeManager *manager,
                        const UmiStudioRuntimeEntry *entry)
{
    UmiIntegrationApplicationState state;

    state = umi_studio_runtime_manager_state(manager, entry->application.id);

    (void)printf("%-30s %-24s %-12s %-13s %zu capabilities\n",
                 entry->application.id,
                 entry->application.name,
                 umi_studio_runtime_category_text(entry->category),
                 umi_integration_application_state_text(state),
                 entry->application.capability_count);
}

static int command_list(UmiStudioRuntimeManager *manager)
{
    size_t index;

    for (index = 0U; index < manager->count; ++index) {
        print_entry(manager, &manager->entries[index]);
    }

    return 0;
}

static int command_summary(UmiStudioRuntimeManager *manager)
{
    UmiStudioRuntimeSnapshot snapshot;

    if (umi_studio_runtime_snapshot_build(manager, &snapshot) != UMI_STATUS_OK) {
        return 1;
    }

    (void)printf("Applications : %zu\n", snapshot.total);
    (void)printf("Installed    : %zu\n", snapshot.installed);
    (void)printf("Running      : %zu\n", snapshot.running);
    (void)printf("Stopped      : %zu\n", snapshot.stopped);
    (void)printf("Failed       : %zu\n", snapshot.failed);
    (void)printf("Favourites   : %zu\n", snapshot.favourites);
    return 0;
}

static int command_show(UmiStudioRuntimeManager *manager, const char *id)
{
    const UmiStudioRuntimeEntry *entry;
    size_t index;

    entry = umi_studio_runtime_manager_find_const(manager, id);
    if (entry == NULL) {
        (void)fprintf(stderr, "Unknown application: %s\n", id);
        return 2;
    }

    print_entry(manager, entry);
    (void)printf("Description : %s\n", entry->description);
    (void)printf("Executable  : %s\n",
                 entry->resolved_executable[0] != '\0'
                     ? entry->resolved_executable
                     : entry->application.executable);
    (void)printf("Favourite   : %s\n", entry->favourite ? "yes" : "no");
    (void)puts("Capabilities:");
    for (index = 0U; index < entry->application.capability_count; ++index) {
        (void)printf("  - %s\n", entry->application.capabilities[index]);
    }
    return 0;
}

static int command_plan(UmiStudioRuntimeManager *manager, const char *id)
{
    UmiStudioRuntimeLaunchPlan plan;
    UmiStatus status;

    status = umi_studio_runtime_plan_launch(manager, id, &plan);
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) {
        return 1;
    }

    (void)printf("Application : %s\n", plan.application_name);
    (void)printf("Readiness   : %s\n",
                 umi_studio_runtime_launch_readiness_text(plan.readiness));
    (void)printf("Executable  : %s\n", plan.executable);
    (void)printf("Can launch  : %s\n", plan.can_launch ? "yes" : "no");
    (void)printf("Reason      : %s\n", plan.reason);
    return status == UMI_STATUS_NOT_FOUND ? 2 : 0;
}

static void usage(void)
{
    (void)puts("Usage:");
    (void)puts("  umicom-studio-apps list");
    (void)puts("  umicom-studio-apps summary");
    (void)puts("  umicom-studio-apps show <application-id>");
    (void)puts("  umicom-studio-apps plan <application-id>");
}

int main(int argc, char **argv)
{
    UmiStudioRuntimeManager manager;
    UmiStatus status;

    umi_studio_runtime_manager_init(&manager);
    status = umi_studio_runtime_catalogue_populate(&manager);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Unable to populate Studio application catalogue.\n");
        return 1;
    }

    if (argc < 2) {
        usage();
        return 0;
    }

    if (strcmp(argv[1], "list") == 0) {
        return command_list(&manager);
    }
    if (strcmp(argv[1], "summary") == 0) {
        return command_summary(&manager);
    }
    if (strcmp(argv[1], "show") == 0 && argc >= 3) {
        return command_show(&manager, argv[2]);
    }
    if (strcmp(argv[1], "plan") == 0 && argc >= 3) {
        return command_plan(&manager, argv[2]);
    }

    usage();
    return 2;
}
