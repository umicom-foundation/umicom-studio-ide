/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/developer_platform.h
 *
 * PURPOSE:
 *   Compose build, testing, terminal, language, debugging and source-control services for Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_DEVELOPER_PLATFORM_H
#define UMICOM_STUDIO_DEVELOPER_PLATFORM_H

#include <stddef.h>

#include "umicom/studio/build.h"
#include "umicom/studio/debugger.h"
#include "umicom/studio/language.h"
#include "umicom/studio/source_control.h"
#include "umicom/studio/terminal.h"
#include "umicom/studio/tests.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioDeveloperPlatform UmiStudioDeveloperPlatform;

typedef struct UmiStudioDeveloperSnapshot {
    UmiStudioBuildSnapshot build;
    UmiStudioTestSnapshot tests;
    UmiStudioTerminalSnapshot terminal;
    UmiStudioLanguageSnapshot language;
    UmiStudioDebuggerSnapshot debugger;
    UmiStudioSourceControlSnapshot source_control;
} UmiStudioDeveloperSnapshot;

UmiStatus umi_studio_developer_platform_create(
    const char *workspace_root,
    UmiClock *clock,
    UmiStudioDeveloperPlatform **out_platform
);
void umi_studio_developer_platform_destroy(
    UmiStudioDeveloperPlatform *platform
);
UmiStatus umi_studio_developer_platform_snapshot(
    const UmiStudioDeveloperPlatform *platform,
    UmiStudioDeveloperSnapshot *out_snapshot
);
UmiStatus umi_studio_developer_platform_report(
    const UmiStudioDeveloperPlatform *platform,
    char *out_text,
    size_t capacity
);
UmiStudioBuildService *umi_studio_developer_platform_build(
    UmiStudioDeveloperPlatform *platform
);
UmiStudioTestService *umi_studio_developer_platform_tests(
    UmiStudioDeveloperPlatform *platform
);
UmiStudioTerminalService *umi_studio_developer_platform_terminal(
    UmiStudioDeveloperPlatform *platform
);
UmiStudioLanguageService *umi_studio_developer_platform_language(
    UmiStudioDeveloperPlatform *platform
);
UmiStudioDebuggerService *umi_studio_developer_platform_debugger(
    UmiStudioDeveloperPlatform *platform
);
UmiStudioSourceControlService *umi_studio_developer_platform_source_control(
    UmiStudioDeveloperPlatform *platform
);

#ifdef __cplusplus
}
#endif

#endif
