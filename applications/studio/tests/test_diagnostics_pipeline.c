/* Verify Studio composes the Framework-owned diagnostics and output pipeline. */
#include <assert.h>
#include <string.h>

#include "umicom/studio/diagnostics.h"

int main(void)
{
    UmiStudioServices *services = NULL;
    UmiBuildResult *result = NULL;
    UmiBuildDiagnostic diagnostic = {0};
    UmiDiagnosticPipelineSnapshot snapshot;
    (void)umi_fs_remove_tree(".umicom");
    assert(umi_build_result_create(&result) == UMI_STATUS_OK);
    assert(umi_studio_services_create(NULL, NULL, &services) == UMI_STATUS_OK);
    umi_build_result_init(result, 77U, UMI_BUILD_PHASE_BUILD, "debug");
    (void)strcpy(result->output, "src/main.c:8:4: error: invalid token\n");
    (void)strcpy(diagnostic.file, "src/main.c");
    (void)strcpy(diagnostic.message, "invalid token");
    diagnostic.line = 8U;
    diagnostic.column = 4U;
    diagnostic.severity = UMI_BUILD_DIAGNOSTIC_ERROR;
    assert(umi_build_diagnostic_list_add(&result->diagnostics, &diagnostic) == UMI_STATUS_OK);
    assert(umi_studio_diagnostics_ingest_build_result(services, result) == UMI_STATUS_OK);
    assert(umi_studio_diagnostics_pipeline_snapshot(services, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.diagnostics.retained_count == 1U);
    assert(snapshot.output.retained_count >= 1U);
    assert(umi_studio_diagnostics_clear_output(services) == UMI_STATUS_OK);
    assert(umi_studio_diagnostics_pipeline_snapshot(services, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.output.retained_count == 0U);
    umi_studio_services_destroy(services);
    umi_build_result_destroy(result);
    assert(umi_fs_remove_tree(".umicom") == UMI_STATUS_OK);
    return 0;
}
