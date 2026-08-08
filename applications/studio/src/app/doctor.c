/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/doctor.c
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

static int make_path(char *buffer, size_t capacity,
                     const char *root, const char *relative)
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

static void report_check(UmiDiagnosticSink sink, void *user_data,
                         UmiStudioDoctorReport *report, int passed,
                         const char *message)
{
    if (passed) {
        ++report->checks_passed;
        umi_diagnostic_emit(sink, user_data, UMI_DIAGNOSTIC_INFO,
                            "studio-doctor", message, 0U);
    } else {
        ++report->checks_failed;
        umi_diagnostic_emit(sink, user_data, UMI_DIAGNOSTIC_ERROR,
                            "studio-doctor", message, 0U);
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
        "applications/studio/CMakeLists.txt",
        "applications/studio/cmake/StudioSources.cmake",
        "applications/studio/src/app/bootstrap.c",
        "applications/studio/src/app/services.c",
        "applications/studio/src/app/version.c",
        "applications/studio/src/app/doctor.c",
        "applications/studio/include/umicom/studio/services.h",
        "applications/studio/include/umicom/studio/version.h",
        "applications/studio/include/umicom/studio/doctor.h",
        "applications/studio/src/tools/doctor_main.c",
        "applications/studio/src/gtk/main.c",
        "applications/studio/application.umicom.yaml",
        "scripts/report-repository-state.ps1",
        "scripts/update-version-lock.ps1",
        "docs/migration/FRAMEWORK_DEPENDENCY_MATRIX_0.11.1.md"
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

    for (index = 0U; index < sizeof(required_files) / sizeof(required_files[0]); ++index) {
        char message[UMI_STUDIO_DOCTOR_PATH_CAPACITY + 64U];
        if (!make_path(path, sizeof(path), repository_root, required_files[index])) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)snprintf(message, sizeof(message), "required file: %s", required_files[index]);
        report_check(sink, user_data, &report, path_exists(path), message);
    }

    for (index = 0U; index < sizeof(forbidden_paths) / sizeof(forbidden_paths[0]); ++index) {
        char message[UMI_STUDIO_DOCTOR_PATH_CAPACITY + 64U];
        if (!make_path(path, sizeof(path), repository_root, forbidden_paths[index])) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)snprintf(message, sizeof(message), "removed migration path: %s", forbidden_paths[index]);
        report_check(sink, user_data, &report, !path_exists(path), message);
    }

    if (!make_path(path, sizeof(path), repository_root,
                   "applications/studio/CMakeLists.txt")) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U; index < sizeof(forbidden_terms) / sizeof(forbidden_terms[0]); ++index) {
        char message[256U];
        (void)snprintf(message, sizeof(message), "active CMake excludes term: %s", forbidden_terms[index]);
        report_check(sink, user_data, &report,
                     text_excludes(path, forbidden_terms[index]), message);
    }

    *out_report = report;
    return UMI_STATUS_OK;
}
