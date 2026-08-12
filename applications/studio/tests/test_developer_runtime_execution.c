/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_runtime_execution.c
 *
 * PURPOSE:
 *   Verify an executor-driven operation flows through Studio's pipeline centre
 *   and automatically reaches the existing progress and output centres.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/studio/developer_workbench.h"

static UmiStatus fake_executor(
    void *user_data,
    const UmiDeveloperOperationSnapshot *operation,
    UmiDeveloperExecutionResult *out_result)
{
    int *count = (int *)user_data;

    assert(operation != NULL);
    *count += 1;
    memset(out_result, 0, sizeof(*out_result));
    out_result->struct_size = (uint32_t)sizeof(*out_result);
    out_result->api_version = UMI_DEVELOPER_EXECUTION_API_VERSION;
    out_result->launched = 1;
    out_result->exit_code = 0;
    strcpy(out_result->output, "Studio developer operation completed.");
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiStudioDeveloperWorkbench *workbench = NULL;
    UmiStudioDeveloperPipelineCentre *centre;
    UmiDeveloperRuntime *runtime;
    UmiDeveloperOperationSnapshot operation;
    UmiDeveloperOperationSnapshot completed;
    UmiDeveloperExecutionResult result;
    UmiDeveloperExecutor executor;
    UmiUiWorkbenchServicesSnapshot services;
    int execution_count = 0;

    assert(umi_studio_developer_workbench_create(&workbench) == UMI_STATUS_OK);
    centre = umi_studio_developer_workbench_pipeline(workbench);
    runtime = umi_studio_developer_workbench_runtime(workbench);

    assert(umi_developer_operation_init(
        &operation,
        "studio.operation",
        UMI_DEVELOPER_OPERATION_BUILD,
        "Studio operation") == UMI_STATUS_OK);
    assert(umi_developer_runtime_submit_operation(runtime, &operation) ==
           UMI_STATUS_OK);

    umi_developer_executor_init(&executor, fake_executor, &execution_count);
    assert(umi_studio_developer_pipeline_centre_execute_next(
        centre, &executor, &completed, &result) == UMI_STATUS_OK);

    assert(execution_count == 1);
    assert(completed.state == UMI_DEVELOPER_OPERATION_SUCCEEDED);
    assert(result.exit_code == 0);

    assert(umi_ui_workbench_services_snapshot(
        umi_studio_developer_workbench_services(workbench),
        &services) == UMI_STATUS_OK);
    assert(services.progress_count >= 1U);
    assert(services.output_channel_count >= 1U);

    umi_studio_developer_workbench_destroy(workbench);
    return 0;
}
