/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/developer_pipeline_main.c
 *
 * PURPOSE:
 *   Provide a native command-line inspection and execution surface for the
 *   Studio developer pipeline.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Without --execute this command only builds and prints the dependency plan.
 * With --execute it runs the ready operations synchronously through Framework's
 * structured process service.  The GUI can use the same runtime from a worker
 * without changing pipeline semantics.
 */
#include "umicom/studio/developer_workbench.h"

#include <stdio.h>
#include <string.h>

static void usage(const char *program)
{
    printf("Usage: %s <source-dir> <build-dir> [configuration] [--execute]\n",
           program);
    printf("       %s --import <project-dir> [--launch <program>] [--execute]\n",
           program);
}


static int run_import_mode(int argc, char **argv)
{
    UmiStudioDeveloperWorkbench *workbench = NULL;
    UmiDeveloperProjectBootstrapRequest request = {0};
    UmiDeveloperProjectBootstrapSnapshot snapshot;
    UmiStudioDeveloperPipelineCentre *centre;
    UmiDeveloperPipeline *pipeline;
    UmiDeveloperOperationSnapshot operation;
    UmiDeveloperExecutionResult result;
    UmiStatus status;
    const char *launch_program = NULL;
    int execute = 0;
    int argument_index;
    size_t operation_index;
    size_t operation_count;

    if (argc < 3) {
        usage(argv[0]);
        return 2;
    }

    for (argument_index = 3; argument_index < argc; ++argument_index) {
        if (strcmp(argv[argument_index], "--execute") == 0) {
            execute = 1;
        } else if (strcmp(argv[argument_index], "--launch") == 0) {
            if (argument_index + 1 >= argc) {
                usage(argv[0]);
                return 2;
            }
            launch_program = argv[++argument_index];
        } else {
            usage(argv[0]);
            return 2;
        }
    }

    status = umi_studio_developer_workbench_create(&workbench);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Failed to create developer workbench: %s\n",
                      umi_status_text(status));
        return 1;
    }

    memset(&request, 0, sizeof(request));
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEVELOPER_PROJECT_BOOTSTRAP_API_VERSION;
    request.project.struct_size = (uint32_t)sizeof(request.project);
    request.project.api_version = UMI_PROJECT_WORKSPACE_IMPORT_API_VERSION;
    request.project.root_directory = argv[2];
    request.project.configuration_name = "Debug";
    request.project.generator = "Ninja";
    request.project.parallel_jobs = 2U;
    request.project.create_test_task = 1;
    request.project.launch_program = launch_program;
    request.preset = launch_program != NULL
                         ? UMI_DEVELOPER_PROJECT_WORKFLOW_FULL
                         : UMI_DEVELOPER_PROJECT_WORKFLOW_TEST;
    request.workflow_id = "studio.import";
    request.prepare_workflow = 1;
    request.include_configure = 1;

    status = umi_studio_developer_workbench_import_project(
        workbench, &request, &snapshot);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Project import failed: %s\n",
                      umi_status_text(status));
        umi_studio_developer_workbench_destroy(workbench);
        return 1;
    }

    (void)printf("Imported project: %s\n", snapshot.project.project_id);
    (void)printf("Root: %s\n", snapshot.project.root_directory);
    (void)printf("Build: %s\n", snapshot.project.build_directory);
    (void)printf("Files: %zu (C=%zu, C++=%zu, headers=%zu)\n",
                 snapshot.project.discovered_file_count,
                 snapshot.project.c_source_count,
                 snapshot.project.cpp_source_count,
                 snapshot.project.header_count);
    (void)printf("CMake: %s  Git: %s  Project validation: %s\n",
                 snapshot.project.has_cmake ? "yes" : "no",
                 snapshot.project.has_git ? "yes" : "no",
                 snapshot.project.validation.valid ? "valid" : "invalid");

    centre = umi_studio_developer_workbench_pipeline(workbench);
    pipeline = umi_developer_runtime_pipeline(
        umi_studio_developer_pipeline_centre_runtime(centre));
    operation_count = umi_developer_pipeline_count(pipeline);
    (void)printf("Developer pipeline: %zu operations\n", operation_count);
    for (operation_index = 0U; operation_index < operation_count;
         ++operation_index) {
        if (umi_developer_pipeline_at(pipeline, operation_index, &operation) ==
            UMI_STATUS_OK) {
            (void)printf("  %s  %-10s  %s\n",
                         operation.id,
                         umi_developer_operation_state_text(operation.state),
                         operation.title);
        }
    }

    if (execute != 0) {
        for (;;) {
            status = umi_studio_developer_pipeline_centre_execute_next_process(
                centre, &operation, &result);
            if (status == UMI_STATUS_NOT_FOUND) {
                break;
            }
            if (status != UMI_STATUS_OK) {
                (void)fprintf(stderr, "Pipeline execution stopped: %s\n",
                              umi_status_text(status));
                umi_studio_developer_workbench_destroy(workbench);
                return 1;
            }
            (void)printf("[%s] %s (exit %d)\n",
                         umi_developer_operation_state_text(operation.state),
                         operation.title,
                         result.exit_code);
            if (operation.state != UMI_DEVELOPER_OPERATION_SUCCEEDED) {
                umi_studio_developer_workbench_destroy(workbench);
                return 1;
            }
        }
    }

    umi_studio_developer_workbench_destroy(workbench);
    return 0;
}

int main(int argc, char **argv)
{
    UmiStudioDeveloperWorkbench *workbench = NULL;
    UmiStudioDeveloperPipelineCentre *centre;
    UmiDeveloperCMakePlanRequest request;
    UmiDeveloperCMakePlanSnapshot plan;
    UmiDeveloperPipeline *pipeline;
    UmiDeveloperOperationSnapshot operation;
    UmiDeveloperExecutionResult result;
    UmiStatus status;
    const char *configuration = "Debug";
    int execute = 0;
    size_t index;
    size_t count;

    if (argc < 3) {
        usage(argv[0]);
        return 2;
    }

    if (strcmp(argv[1], "--import") == 0) {
        return run_import_mode(argc, argv);
    }

    if (argc >= 4 && strcmp(argv[3], "--execute") != 0) {
        configuration = argv[3];
    }
    if ((argc >= 4 && strcmp(argv[3], "--execute") == 0) ||
        (argc >= 5 && strcmp(argv[4], "--execute") == 0)) {
        execute = 1;
    }

    status = umi_studio_developer_workbench_create(&workbench);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Failed to create developer workbench: %s\n",
                umi_status_text(status));
        return 1;
    }

    centre = umi_studio_developer_workbench_pipeline(workbench);
    pipeline = umi_developer_runtime_pipeline(
        umi_studio_developer_pipeline_centre_runtime(centre));

    memset(&request, 0, sizeof(request));
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEVELOPER_CMAKE_PLAN_API_VERSION;
    request.plan_id = "studio.cli";
    request.project_id = "umicom-studio";
    request.configuration_id = configuration;
    request.source_directory = argv[1];
    request.build_directory = argv[2];
    request.generator = "Ninja";
    request.parallel_jobs = 2U;
    request.include_configure = 1;
    request.include_build = 1;
    request.include_tests = 1;

    status = umi_studio_developer_pipeline_centre_prepare_cmake(
        centre, &request, &plan);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Failed to prepare pipeline: %s\n",
                umi_status_text(status));
        umi_studio_developer_workbench_destroy(workbench);
        return 1;
    }

    printf("Developer pipeline: %zu operations\n", plan.operation_count);
    count = umi_developer_pipeline_count(pipeline);
    for (index = 0U; index < count; ++index) {
        if (umi_developer_pipeline_at(pipeline, index, &operation) ==
            UMI_STATUS_OK) {
            printf("  %s  %-10s  %s\n",
                   operation.id,
                   umi_developer_operation_state_text(operation.state),
                   operation.title);
        }
    }

    if (execute != 0) {
        for (;;) {
            status = umi_studio_developer_pipeline_centre_execute_next_process(
                centre, &operation, &result);
            if (status == UMI_STATUS_NOT_FOUND) {
                break;
            }
            if (status != UMI_STATUS_OK) {
                fprintf(stderr, "Pipeline execution stopped: %s\n",
                        umi_status_text(status));
                umi_studio_developer_workbench_destroy(workbench);
                return 1;
            }

            printf("[%s] %s (exit %d)\n",
                   umi_developer_operation_state_text(operation.state),
                   operation.title,
                   result.exit_code);

            if (operation.state != UMI_DEVELOPER_OPERATION_SUCCEEDED) {
                break;
            }
        }
    }

    umi_studio_developer_workbench_destroy(workbench);
    return 0;
}
