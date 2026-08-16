/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/debug_orchestration_contribution.c
 *
 * PURPOSE:
 *   Place Framework debugger contracts in Studio menus and workbench regions.
 *   All operational behaviour remains inside Umicom Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/debug_orchestration_contribution.h"

#include <string.h>

#define COMMAND(command, menu, group, position)                              \
    {                                                                         \
        (uint32_t)sizeof(UmiStudioDebugCommandContribution),                  \
        UMI_STUDIO_DEBUG_ORCHESTRATION_CONTRIBUTION_API_VERSION,              \
        command, menu, group, position, 1                                     \
    }

static const UmiStudioDebugCommandContribution COMMANDS[] = {
    COMMAND("debug.start", "Run", "session", 100),
    COMMAND("debug.start-without-debugging", "Run", "session", 110),
    COMMAND("debug.configuration.select", "Run", "configuration", 200),
    COMMAND("debug.configuration.open", "Run", "configuration", 210),
    COMMAND("debug.restart", "Run", "session", 120),
    COMMAND("debug.stop", "Run", "session", 130),
    COMMAND("debug.continue", "Run", "stepping", 300),
    COMMAND("debug.pause", "Run", "stepping", 310),
    COMMAND("debug.step-over", "Run", "stepping", 320),
    COMMAND("debug.step-into", "Run", "stepping", 330),
    COMMAND("debug.step-out", "Run", "stepping", 340),
    COMMAND("debug.run-to-cursor", "Run", "stepping", 350),
    COMMAND("debug.breakpoint.toggle", "Run", "breakpoints", 400),
    COMMAND("debug.breakpoint.enable-all", "Run", "breakpoints", 410),
    COMMAND("debug.breakpoint.disable-all", "Run", "breakpoints", 420),
    COMMAND("debug.breakpoint.remove-all", "Run", "breakpoints", 430),
    COMMAND("debug.breakpoint.add-conditional", "Run", "breakpoints", 440),
    COMMAND("debug.breakpoint.add-logpoint", "Run", "breakpoints", 450),
    COMMAND("debug.watch.add", "Run", "evaluation", 500),
    COMMAND("debug.watch.remove", "Run", "evaluation", 510),
    COMMAND("debug.evaluate-selection", "Run", "evaluation", 520),
    COMMAND("debug.console.open", "View", "debug", 600),
    COMMAND("debug.console.clear", "Run", "console", 610),
    COMMAND("debug.call-stack.copy", "Run", "inspection", 700),
    COMMAND("debug.disassembly.open", "View", "debug", 710),
    COMMAND("debug.memory.open", "View", "debug", 720)
};

#undef COMMAND

#define VIEW(view, label, contract, region, position)                        \
    {                                                                         \
        (uint32_t)sizeof(UmiStudioDebugViewContribution),                     \
        UMI_STUDIO_DEBUG_ORCHESTRATION_CONTRIBUTION_API_VERSION,              \
        view, label, contract, region, position, 1, 1                         \
    }

static const UmiStudioDebugViewContribution VIEWS[] = {
    VIEW("studio.debug.configuration-centre", "Debug Configurations",
         "umicom.debug.configuration-resolver", "left-sidebar", 80),
    VIEW("studio.debug.compound-sessions", "Compound Debug Sessions",
         "umicom.debug.compound-configuration", "left-sidebar", 90),
    VIEW("studio.debug.query-filters", "Debug Filters",
         "umicom.debug.breakpoint-watch-console-query", "bottom-panel", 120),
    VIEW("studio.debug.session-orchestration", "Debug Session",
         "umicom.debug.orchestration", "bottom-panel", 130)
};

#undef VIEW

size_t umi_studio_debug_command_contribution_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiStudioDebugCommandContribution *
umi_studio_debug_command_contribution_at(size_t index)
{
    return index < umi_studio_debug_command_contribution_count()
        ? &COMMANDS[index] : NULL;
}

const UmiStudioDebugCommandContribution *
umi_studio_debug_command_contribution_find(const char *framework_command_id)
{
    size_t index;
    if (framework_command_id == NULL) return NULL;
    for (index = 0U; index < umi_studio_debug_command_contribution_count();
         ++index) {
        if (strcmp(COMMANDS[index].framework_command_id,
                   framework_command_id) == 0) return &COMMANDS[index];
    }
    return NULL;
}

size_t umi_studio_debug_view_contribution_count(void)
{
    return sizeof(VIEWS) / sizeof(VIEWS[0]);
}

const UmiStudioDebugViewContribution *
umi_studio_debug_view_contribution_at(size_t index)
{
    return index < umi_studio_debug_view_contribution_count()
        ? &VIEWS[index] : NULL;
}

const UmiStudioDebugViewContribution *
umi_studio_debug_view_contribution_find(const char *view_id)
{
    size_t index;
    if (view_id == NULL) return NULL;
    for (index = 0U; index < umi_studio_debug_view_contribution_count();
         ++index) {
        if (strcmp(VIEWS[index].view_id, view_id) == 0) return &VIEWS[index];
    }
    return NULL;
}
