/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/build_workbench.c
 *
 * PURPOSE:
 *   Publish structured compiler/test diagnostics and raw output to the
 *   toolkit-neutral Studio workbench registries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/build_workbench.h"

UmiStatus umi_studio_build_publish_result(
    const UmiBuildResult *result,
    UmiUiProblemRegistry *problems,
    UmiUiOutputChannelRegistry *output_channels)
{
    UmiDiagnosticPipeline *pipeline = NULL;
    UmiDiagnosticPipelineConfig config;
    UmiStatus status;
    if (result == NULL || problems == NULL || output_channels == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    config = umi_diagnostic_pipeline_config_default();
    config.mirror_diagnostics_to_output = 0;
    status = umi_diagnostic_pipeline_create(&config, &pipeline);
    if (status == UMI_STATUS_OK) {
        status = umi_diagnostic_build_result_ingest(pipeline, result, "Umicom Build");
    }
    if (status == UMI_STATUS_OK) {
        status = umi_diagnostic_ui_sync_registries(pipeline, problems, output_channels);
    }
    umi_diagnostic_pipeline_destroy(pipeline);
    return status;
}
