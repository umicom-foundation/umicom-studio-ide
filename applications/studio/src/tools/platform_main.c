/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/platform_main.c
 *
 * PURPOSE:
 *   Provide a native Studio platform report that calls the same Framework
 *   services used by the `umicom` command and future GTK4 environment page.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/platform.h"

#include <stdio.h>
#include <string.h>

static void console_sink(const UmiDiagnostic *diagnostic, void *user_data)
{
    (void)user_data;
    if (diagnostic != NULL) {
        (void)printf("[%s][%s] %s\n",
                     umi_diagnostic_severity_text(diagnostic->severity),
                     diagnostic->source != NULL
                         ? diagnostic->source : "studio-platform",
                     diagnostic->message != NULL ? diagnostic->message : "");
    }
}

int main(int argc, char **argv)
{
    UmiStudioServices *services = NULL;
    UmiStudioPlatformReport report;
    UmiStatus status;
    int require_gtk = 0;
    int require_github = 0;
    int index;

    for (index = 1; index < argc; ++index) {
        if (strcmp(argv[index], "--gtk") == 0) require_gtk = 1;
        if (strcmp(argv[index], "--github") == 0) require_github = 1;
    }
    status = umi_studio_services_create(console_sink, NULL, &services);
    if (status != UMI_STATUS_OK) return 1;
    status = umi_studio_platform_check(services,
                                       require_gtk,
                                       require_github,
                                       &report);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Platform check failed: %s\n",
                      umi_status_text(status));
        umi_studio_services_destroy(services);
        return 1;
    }

    (void)printf("Profile: %s\n", report.discovery.profile.profile_id);
    (void)printf("Family: %s\n",
                 umi_toolchain_family_text(report.discovery.profile.family));
    (void)printf("Tools: %zu/%zu\n",
                 report.discovery.tools_found,
                 report.discovery.required_tools);
    (void)printf("Compile probe: %s\n",
                 report.discovery.compile_probe_passed ? "PASS" : "FAIL");
    (void)printf("Link probe: %s\n",
                 report.discovery.link_probe_passed ? "PASS" : "FAIL");
    (void)printf("Runtime probe: %s\n",
                 report.discovery.runtime_probe_passed ? "PASS" : "FAIL");
    (void)printf("Framework capabilities: %zu\n",
                 report.framework_capability_count);

    umi_studio_platform_report_dispose(&report);
    umi_studio_services_destroy(services);
    return 0;
}
