/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_build_workbench.c
 * PURPOSE: Verify build results populate Problems and Output registries.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/build_workbench.h"

int main(void)
{
    UmiUiProblemRegistry *problems = NULL;
    UmiUiOutputChannelRegistry *outputs = NULL;
    UmiBuildResult result;
    UmiBuildDiagnostic diagnostic = {0};
    assert(umi_ui_problem_registry_create(&problems) == UMI_STATUS_OK);
    assert(umi_ui_output_channel_registry_create(&outputs) == UMI_STATUS_OK);
    umi_build_result_init(&result, 42U, UMI_BUILD_PHASE_BUILD, "debug");
    strcpy(result.output, "src/main.c:7:3: error: bad token\n");
    strcpy(diagnostic.file, "src/main.c");
    strcpy(diagnostic.message, "bad token");
    diagnostic.line = 7U;
    diagnostic.column = 3U;
    diagnostic.severity = UMI_BUILD_DIAGNOSTIC_ERROR;
    assert(umi_build_diagnostic_list_add(&result.diagnostics, &diagnostic) ==
           UMI_STATUS_OK);
    assert(umi_studio_build_publish_result(&result, problems, outputs) ==
           UMI_STATUS_OK);
    assert(umi_ui_problem_registry_count(problems) == 1U);
    assert(umi_ui_output_channel_registry_count(outputs) == 1U);
    umi_ui_output_channel_registry_destroy(outputs);
    umi_ui_problem_registry_destroy(problems);
    return 0;
}
