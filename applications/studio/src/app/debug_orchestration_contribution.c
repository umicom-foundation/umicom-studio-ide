/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/debug_orchestration_contribution.c
 *
 * PURPOSE:
 *   Place Framework debugger contracts in Studio menus and workbench regions.
 *   All configuration, breakpoint, thread, register, memory, disassembly and
 *   adapter behaviour remains authoritative inside Umicom Framework.
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
    COMMAND("debug.memory.open", "View", "debug", 720),
    COMMAND("debug.breakpoint.add-function", "Run", "advanced-breakpoints", 500),
    COMMAND("debug.breakpoint.add-data", "Run", "advanced-breakpoints", 510),
    COMMAND("debug.breakpoint.add-instruction", "Run", "advanced-breakpoints", 520),
    COMMAND("debug.breakpoint.configure-exceptions", "Run", "advanced-breakpoints", 530),
    COMMAND("debug.breakpoints.open-advanced", "View", "debug", 730),
    COMMAND("debug.threads.open", "View", "debug", 740),
    COMMAND("debug.thread.select", "Run", "inspection", 710),
    COMMAND("debug.frame.select", "Run", "inspection", 720),
    COMMAND("debug.threads.refresh", "Run", "inspection", 730),
    COMMAND("debug.variables.refresh", "Run", "variables", 740),
    COMMAND("debug.variable.set", "Run", "variables", 750),
    COMMAND("debug.expression.set", "Run", "variables", 760),
    COMMAND("debug.step-back", "Run", "reverse", 360),
    COMMAND("debug.reverse-continue", "Run", "reverse", 370),
    COMMAND("debug.frame.restart", "Run", "reverse", 380),
    COMMAND("debug.modules.open", "View", "debug", 750),
    COMMAND("debug.registers.open", "View", "debug", 760),
    COMMAND("debug.registers.refresh", "Run", "inspection", 770),
    COMMAND("debug.memory.refresh", "Run", "inspection", 780),
    COMMAND("debug.memory.write", "Run", "inspection", 790),
    COMMAND("debug.disassembly.refresh", "Run", "inspection", 800),
    COMMAND("debug.breakpoint.toggle-instruction", "Run", "advanced-breakpoints", 540),
    COMMAND("debug.adapters.open", "Tools", "debugging", 900),
    COMMAND("debug.memory.copy-address", "Edit", "debugging", 900)
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
         "umicom.debug.orchestration", "bottom-panel", 130),
    VIEW("studio.debug.advanced-breakpoints", "Advanced Breakpoints",
         "umicom.debug.advanced-breakpoint", "left-sidebar", 100),
    VIEW("studio.debug.threads", "Threads and Call Stack",
         "umicom.debug.thread-inspector", "left-sidebar", 110),
    VIEW("studio.debug.registers", "Registers",
         "umicom.debug.register-bank", "bottom-panel", 140),
    VIEW("studio.debug.memory", "Memory",
         "umicom.debug.memory-view", "editor-area", 150),
    VIEW("studio.debug.disassembly", "Disassembly",
         "umicom.debug.disassembly-view", "editor-area", 160),
    VIEW("studio.debug.modules", "Modules",
         "umicom.debug.module-registry", "bottom-panel", 170),
    VIEW("studio.debug.adapter-centre", "Debugger Adapters",
         "umicom.debug.adapter-contract", "secondary-sidebar", 180),
    VIEW("studio.debug.inspection-session", "Advanced Debug Inspection",
         "umicom.debug.inspection-session", "bottom-panel", 190)
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
