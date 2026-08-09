/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/diagnostics_main.c
 *
 * PURPOSE:
 *   Provide a native C23 command for inspecting retained Framework diagnostics
 *   without starting GTK4.  The command supports severity filtering, a result
 *   limit, and deterministic demonstration records for validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/diagnostics.h"

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_STUDIO_DIAGNOSTICS_DEFAULT_LIMIT 50U

static void print_usage(const char *program)
{
    (void)printf(
        "Usage: %s [--min <severity>] [--limit <number>] [--demo]\n"
        "\n"
        "Severity values: trace, info, warning, error, fatal\n"
        "--demo adds one warning and one error for a visible smoke test.\n",
        program != NULL ? program : "umicom-studio-diagnostics"
    );
}

static int text_equals_ignore_case(const char *left, const char *right)
{
    if (left == NULL || right == NULL) {
        return 0;
    }

    while (*left != '\0' && *right != '\0') {
        unsigned char left_character = (unsigned char)*left;
        unsigned char right_character = (unsigned char)*right;
        if (tolower(left_character) != tolower(right_character)) {
            return 0;
        }
        ++left;
        ++right;
    }

    return *left == '\0' && *right == '\0';
}

static int parse_severity(const char *text,
                          UmiDiagnosticSeverity *out_severity)
{
    if (out_severity == NULL) {
        return 0;
    }

    if (text_equals_ignore_case(text, "trace")) {
        *out_severity = UMI_DIAGNOSTIC_TRACE;
    } else if (text_equals_ignore_case(text, "info")) {
        *out_severity = UMI_DIAGNOSTIC_INFO;
    } else if (text_equals_ignore_case(text, "warning") ||
               text_equals_ignore_case(text, "warn")) {
        *out_severity = UMI_DIAGNOSTIC_WARNING;
    } else if (text_equals_ignore_case(text, "error")) {
        *out_severity = UMI_DIAGNOSTIC_ERROR;
    } else if (text_equals_ignore_case(text, "fatal")) {
        *out_severity = UMI_DIAGNOSTIC_FATAL;
    } else {
        return 0;
    }

    return 1;
}

static int parse_limit(const char *text, size_t *out_limit)
{
    char *end = NULL;
    unsigned long long value;

    if (text == NULL || out_limit == NULL || *text == '\0') {
        return 0;
    }

    errno = 0;
    value = strtoull(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value > SIZE_MAX) {
        return 0;
    }

    *out_limit = (size_t)value;
    return 1;
}

static int severity_is_at_least(UmiDiagnosticSeverity severity,
                                UmiDiagnosticSeverity minimum)
{
    return (int)severity >= (int)minimum;
}

static void print_summary(const UmiDiagnosticStoreSummary *summary)
{
    (void)printf(
        "Diagnostics: retained=%zu capacity=%zu received=%" PRIu64
        " overwritten=%" PRIu64 "\n",
        summary->retained_count,
        summary->capacity,
        summary->total_received,
        summary->overwritten_count
    );
    (void)printf(
        "Severity counts: trace=%" PRIu64 " info=%" PRIu64
        " warning=%" PRIu64 " error=%" PRIu64 " fatal=%" PRIu64 "\n",
        summary->retained_by_severity[0],
        summary->retained_by_severity[1],
        summary->retained_by_severity[2],
        summary->retained_by_severity[3],
        summary->retained_by_severity[4]
    );
}

static UmiStatus print_records(UmiStudioServices *services,
                               UmiDiagnosticSeverity minimum,
                               size_t limit)
{
    size_t count = umi_studio_diagnostics_count(services);
    size_t matching = 0U;
    size_t skip;
    size_t index;

    for (index = 0U; index < count; ++index) {
        UmiDiagnosticRecord record;
        UmiStatus status = umi_studio_diagnostics_record_at(
            services,
            index,
            &record
        );
        if (status != UMI_STATUS_OK) {
            return status;
        }
        if (severity_is_at_least(record.severity, minimum)) {
            ++matching;
        }
    }

    skip = matching > limit ? matching - limit : 0U;

    for (index = 0U; index < count; ++index) {
        UmiDiagnosticRecord record;
        UmiStatus status = umi_studio_diagnostics_record_at(
            services,
            index,
            &record
        );
        if (status != UMI_STATUS_OK) {
            return status;
        }
        if (!severity_is_at_least(record.severity, minimum)) {
            continue;
        }
        if (skip > 0U) {
            --skip;
            continue;
        }

        (void)printf(
            "\n[%" PRIu64 "][%s][%s][correlation=%" PRIu64 "]\n%s\n",
            record.sequence,
            umi_diagnostic_severity_text(record.severity),
            record.source,
            record.correlation_id,
            record.message
        );
    }

    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    UmiDiagnosticSeverity minimum = UMI_DIAGNOSTIC_TRACE;
    size_t limit = UMI_STUDIO_DIAGNOSTICS_DEFAULT_LIMIT;
    int include_demo = 0;
    int index;
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioServices *services;
    UmiDiagnosticStoreSummary summary;
    UmiStatus status;

    for (index = 1; index < argc; ++index) {
        if (strcmp(argv[index], "--help") == 0 ||
            strcmp(argv[index], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        if (strcmp(argv[index], "--demo") == 0) {
            include_demo = 1;
            continue;
        }
        if (strcmp(argv[index], "--min") == 0 && index + 1 < argc) {
            ++index;
            if (!parse_severity(argv[index], &minimum)) {
                (void)fprintf(stderr,
                              "Invalid severity: %s\n",
                              argv[index]);
                return 2;
            }
            continue;
        }
        if (strcmp(argv[index], "--limit") == 0 && index + 1 < argc) {
            ++index;
            if (!parse_limit(argv[index], &limit)) {
                (void)fprintf(stderr,
                              "Invalid limit: %s\n",
                              argv[index]);
                return 2;
            }
            continue;
        }

        (void)fprintf(stderr, "Unknown argument: %s\n", argv[index]);
        print_usage(argv[0]);
        return 2;
    }

    status = umi_studio_bootstrap_create(&bootstrap);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Studio create failed: %s\n",
                      umi_status_text(status));
        return 1;
    }

    status = umi_studio_bootstrap_start(bootstrap);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Studio start failed: %s\n",
                      umi_status_text(status));
        umi_studio_bootstrap_destroy(bootstrap);
        return 1;
    }

    services = umi_studio_bootstrap_services(bootstrap);
    if (include_demo) {
        (void)umi_studio_diagnostics_emit(
            services,
            UMI_DIAGNOSTIC_WARNING,
            "studio-diagnostics",
            "Demonstration warning record",
            12001U
        );
        (void)umi_studio_diagnostics_emit(
            services,
            UMI_DIAGNOSTIC_ERROR,
            "studio-diagnostics",
            "Demonstration error record",
            12002U
        );
    }

    status = umi_studio_diagnostics_summary(services, &summary);
    if (status == UMI_STATUS_OK) {
        print_summary(&summary);
        status = print_records(services, minimum, limit);
    }

    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Diagnostics command failed: %s\n",
                      umi_status_text(status));
        umi_studio_bootstrap_destroy(bootstrap);
        return 1;
    }

    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}
