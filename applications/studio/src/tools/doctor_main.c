/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/doctor_main.c
 *---------------------------------------------------------------------------*/
#include "umicom/studio/doctor.h"

#include <stdio.h>

static void console_sink(const UmiDiagnostic *diagnostic, void *user_data)
{
    (void)user_data;
    if (diagnostic == NULL) {
        return;
    }
    (void)printf("[%s] %s\n",
                 umi_diagnostic_severity_text(diagnostic->severity),
                 diagnostic->message != NULL ? diagnostic->message : "");
}

int main(int argc, char **argv)
{
    const char *root = argc > 1 ? argv[1] : ".";
    UmiStudioDoctorReport report;
    UmiStatus status = umi_studio_doctor_run(root, console_sink, NULL, &report);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Doctor failed to run: %s\n", umi_status_text(status));
        return 2;
    }
    (void)printf("Checks passed: %zu\n", report.checks_passed);
    (void)printf("Checks failed: %zu\n", report.checks_failed);
    return report.checks_failed == 0U ? 0 : 1;
}
