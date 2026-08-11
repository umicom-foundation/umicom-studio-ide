/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/runtime-manager/src/runtime_launch_plan.c
 *
 * PURPOSE:
 *   Create a safe non-mutating launch plan for one catalogued application.
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

#include "umicom/studio/runtime_launch_plan.h"

#include <stdio.h>
#include <string.h>

static void set_text(char *destination, size_t capacity, const char *source)
{
    if (destination == NULL || capacity == 0U) {
        return;
    }

    (void)snprintf(destination,
                   capacity,
                   "%s",
                   source != NULL ? source : "");
}

UmiStatus umi_studio_runtime_plan_launch(
    const UmiStudioRuntimeManager *manager,
    const char *application_id,
    UmiStudioRuntimeLaunchPlan *plan)
{
    const UmiStudioRuntimeEntry *entry;
    UmiIntegrationApplicationState state;

    if (manager == NULL || application_id == NULL || plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(plan, 0, sizeof(*plan));
    entry = umi_studio_runtime_manager_find_const(manager, application_id);

    if (entry == NULL) {
        plan->readiness = UMI_STUDIO_RUNTIME_LAUNCH_APPLICATION_UNKNOWN;
        set_text(plan->application_id,
                 sizeof(plan->application_id),
                 application_id);
        set_text(plan->reason,
                 sizeof(plan->reason),
                 "Application is not registered in the Studio catalogue.");
        return UMI_STATUS_NOT_FOUND;
    }

    set_text(plan->application_id,
             sizeof(plan->application_id),
             entry->application.id);
    set_text(plan->application_name,
             sizeof(plan->application_name),
             entry->application.name);

    state = umi_studio_runtime_manager_state(manager, application_id);
    if (state == UMI_INTEGRATION_APP_RUNNING) {
        plan->readiness = UMI_STUDIO_RUNTIME_LAUNCH_ALREADY_RUNNING;
        set_text(plan->reason,
                 sizeof(plan->reason),
                 "Application is already running.");
        return UMI_STATUS_OK;
    }

    if (entry->resolved_executable[0] != '\0') {
        set_text(plan->executable,
                 sizeof(plan->executable),
                 entry->resolved_executable);
    } else {
        set_text(plan->executable,
                 sizeof(plan->executable),
                 entry->application.executable);
    }

    if (plan->executable[0] == '\0') {
        plan->readiness = UMI_STUDIO_RUNTIME_LAUNCH_EXECUTABLE_UNKNOWN;
        set_text(plan->reason,
                 sizeof(plan->reason),
                 "No executable has been resolved for this application.");
        return UMI_STATUS_OK;
    }

    plan->readiness = UMI_STUDIO_RUNTIME_LAUNCH_READY;
    plan->can_launch = true;
    set_text(plan->reason,
             sizeof(plan->reason),
             "Launch plan is ready. Process execution remains explicit.");
    return UMI_STATUS_OK;
}

const char *umi_studio_runtime_launch_readiness_text(
    UmiStudioRuntimeLaunchReadiness readiness)
{
    switch (readiness) {
        case UMI_STUDIO_RUNTIME_LAUNCH_READY: return "Ready";
        case UMI_STUDIO_RUNTIME_LAUNCH_ALREADY_RUNNING:
            return "Already running";
        case UMI_STUDIO_RUNTIME_LAUNCH_EXECUTABLE_UNKNOWN:
            return "Executable unknown";
        case UMI_STUDIO_RUNTIME_LAUNCH_APPLICATION_UNKNOWN:
            return "Application unknown";
        default:
            return "Unknown";
    }
}
