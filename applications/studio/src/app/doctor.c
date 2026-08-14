/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/doctor.c
 *
 * PURPOSE:
 *   Inspect a Studio source checkout, verify required Framework-based files,
 *   and confirm that obsolete migration paths and CMake identifiers are not
 *   active.  Results are emitted through the Framework diagnostic contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/doctor.h"

#include "umicom/platform/filesystem.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define UMI_STUDIO_DOCTOR_PATH_CAPACITY 2048U

static int make_path(char *buffer,
                     size_t capacity,
                     const char *root,
                     const char *relative)
{
    int written;
    if (buffer == NULL || capacity == 0U || root == NULL || relative == NULL) {
        return 0;
    }
    written = snprintf(buffer, capacity, "%s/%s", root, relative);
    return written >= 0 && (size_t)written < capacity;
}

static int path_exists(const char *path)
{
    struct stat information;
    return path != NULL && stat(path, &information) == 0;
}

static void report_check(UmiDiagnosticSink sink,
                         void *user_data,
                         UmiStudioDoctorReport *report,
                         int passed,
                         const char *message)
{
    if (passed) {
        ++report->checks_passed;
        umi_diagnostic_emit(sink,
                            user_data,
                            UMI_DIAGNOSTIC_INFO,
                            "studio-doctor",
                            message,
                            0U);
    } else {
        ++report->checks_failed;
        umi_diagnostic_emit(sink,
                            user_data,
                            UMI_DIAGNOSTIC_ERROR,
                            "studio-doctor",
                            message,
                            0U);
    }
}

static int text_excludes(const char *path, const char *needle)
{
    char *text = NULL;
    size_t size = 0U;
    UmiStatus status = umi_fs_read_text(path, &text, &size);
    int result;
    (void)size;

    if (status != UMI_STATUS_OK || text == NULL) {
        return 0;
    }

    result = strstr(text, needle) == NULL;
    umi_fs_free_text(text);
    return result;
}

UmiStatus umi_studio_doctor_run(const char *repository_root,
                                UmiDiagnosticSink sink,
                                void *user_data,
                                UmiStudioDoctorReport *out_report)
{
    static const char *required_files[] = {
        "CMakeLists.txt",
        "CMakePresets.json",
        "framework/CMakeLists.txt",
        "framework/include/umicom/base/version.h",
        "framework/include/umicom/runtime/capability_registry.h",
        "framework/include/umicom/runtime/capability_catalogue.h",
        "framework/include/umicom/runtime/application_manifest.h",
        "framework/include/umicom/runtime/suite.h",
        "framework/include/umicom/security/policy.h",
        "framework/include/umicom/security/secrets.h",
        "framework/include/umicom/toolchain/discovery.h",
        "framework/include/umicom/toolchain/environment.h",
        "framework/include/umicom/toolchain/build.h",
        "framework/include/umicom/repository/repository.h",
        "framework/include/umicom/scaffold/repository.h",
        "framework/tools/umicom/src/main.c",
        "framework/templates/repository/CMakeLists.txt.in",
        "framework/tests/test_repository_scaffold.c",
        "applications/studio/CMakeLists.txt",
        "applications/studio/cmake/StudioSources.cmake",
        "applications/studio/src/app/bootstrap.c",
        "applications/studio/src/app/services.c",
        "applications/studio/src/app/platform.c",
        "applications/studio/src/app/operations.c",
        "applications/studio/src/app/security.c",
        "applications/studio/src/app/plugins.c",
        "applications/studio/src/app/observability.c",
        "applications/studio/src/app/resilience.c",
        "applications/studio/include/umicom/studio/platform.h",
        "applications/studio/include/umicom/studio/operations.h",
        "applications/studio/include/umicom/studio/security.h",
        "applications/studio/include/umicom/studio/plugins.h",
        "applications/studio/include/umicom/studio/observability.h",
        "applications/studio/include/umicom/studio/resilience.h",
        "applications/studio/src/tools/platform_main.c",
        "applications/studio/tests/test_platform.c",
        "applications/studio/src/gtk/main.c",
        "applications/studio/application.umicom.yaml",
        "docs/adr/ADR-0006-native-framework-platform-and-major-release-train.md"
    };
    static const char *forbidden_paths[] = {
        "applications/studio/src/legacy",
        "applications/studio/cmake/LegacySources.cmake"
    };
    static const char *forbidden_terms[] = {
        "StudioLegacy",
        "LegacySources.cmake",
        "src/legacy",
        "UMICOM_STUDIO_BUILD_LEGACY_GTK"
    };
    char path[UMI_STUDIO_DOCTOR_PATH_CAPACITY];
    UmiStudioDoctorReport report = {0U, 0U};
    size_t index;

    if (repository_root == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U;
         index < sizeof(required_files) / sizeof(required_files[0]);
         ++index) {
        char message[UMI_STUDIO_DOCTOR_PATH_CAPACITY + 64U];
        if (!make_path(path,
                       sizeof(path),
                       repository_root,
                       required_files[index])) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)snprintf(message,
                       sizeof(message),
                       "required file: %s",
                       required_files[index]);
        report_check(sink,
                     user_data,
                     &report,
                     path_exists(path),
                     message);
    }

    for (index = 0U;
         index < sizeof(forbidden_paths) / sizeof(forbidden_paths[0]);
         ++index) {
        char message[UMI_STUDIO_DOCTOR_PATH_CAPACITY + 64U];
        if (!make_path(path,
                       sizeof(path),
                       repository_root,
                       forbidden_paths[index])) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)snprintf(message,
                       sizeof(message),
                       "removed migration path: %s",
                       forbidden_paths[index]);
        report_check(sink,
                     user_data,
                     &report,
                     !path_exists(path),
                     message);
    }

    if (!make_path(path,
                   sizeof(path),
                   repository_root,
                   "applications/studio/CMakeLists.txt")) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    for (index = 0U;
         index < sizeof(forbidden_terms) / sizeof(forbidden_terms[0]);
         ++index) {
        char message[256U];
        (void)snprintf(message,
                       sizeof(message),
                       "active CMake excludes term: %s",
                       forbidden_terms[index]);
        report_check(sink,
                     user_data,
                     &report,
                     text_excludes(path, forbidden_terms[index]),
                     message);
    }

    *out_report = report;
    return UMI_STATUS_OK;
}
