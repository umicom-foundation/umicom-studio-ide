/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_platform.c
 *
 * PURPOSE:
 *   Verify composition and reporting across all Framework-backed Studio developer services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/developer_platform.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiStudioDeveloperPlatform *platform = NULL;
    UmiStudioDeveloperSnapshot snapshot;
    UmiClock clock = umi_clock_system();
    char report[4096];
    assert(umi_studio_developer_platform_create(".", &clock, &platform) == UMI_STATUS_OK);
    assert(umi_studio_developer_platform_snapshot(platform, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.terminal.session_count == 1U);
    assert(snapshot.tests.suite_count == 1U);
    assert(umi_studio_developer_platform_report(platform,
                                                 report,
                                                 sizeof(report)) == UMI_STATUS_OK);
    assert(strstr(report, "Build profile:") != NULL);
    assert(strstr(report, "Source control:") != NULL);
    assert(umi_studio_developer_platform_build(platform) != NULL);
    assert(umi_studio_developer_platform_tests(platform) != NULL);
    assert(umi_studio_developer_platform_terminal(platform) != NULL);
    assert(umi_studio_developer_platform_language(platform) != NULL);
    assert(umi_studio_developer_platform_debugger(platform) != NULL);
    assert(umi_studio_developer_platform_source_control(platform) != NULL);
    umi_studio_developer_platform_destroy(platform);
    umi_clock_dispose(&clock);
    return 0;
}
