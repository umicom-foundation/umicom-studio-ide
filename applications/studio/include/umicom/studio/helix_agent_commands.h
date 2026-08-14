/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/helix_agent_commands.h
 * PURPOSE: Expose controlled Helix v2 plan, approval, run and rollback commands.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Command handlers stay thin and call the same centre used by
 * native tools and future graphical workbench actions. */
#ifndef UMICOM_STUDIO_HELIX_AGENT_COMMANDS_H
#define UMICOM_STUDIO_HELIX_AGENT_COMMANDS_H

#include "umicom/studio/helix_agent_centre.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_helix_agent_plan(UmiStudioHelixAgentCentre *centre,
                                      const char *operation_id,
                                      const char *objective,
                                      const char *plan_hash,
                                      const UmiHelixAction *action);
UmiStatus umi_studio_helix_agent_approve(UmiStudioHelixAgentCentre *centre,
                                         const char *approver,
                                         const char *reason);
UmiStatus umi_studio_helix_agent_reject(UmiStudioHelixAgentCentre *centre,
                                        const char *approver,
                                        const char *reason);
UmiStatus umi_studio_helix_agent_run(UmiStudioHelixAgentCentre *centre,
                                     char *out_evidence,
                                     size_t capacity);
UmiStatus umi_studio_helix_agent_add_compensation(
    UmiStudioHelixAgentCentre *centre,
    const UmiHelixAction *action);
UmiStatus umi_studio_helix_agent_begin_rollback(
    UmiStudioHelixAgentCentre *centre,
    int human_approved);
UmiStatus umi_studio_helix_agent_run_rollback(
    UmiStudioHelixAgentCentre *centre,
    char *out_evidence,
    size_t capacity);
UmiStatus umi_studio_helix_agent_status(UmiStudioHelixAgentCentre *centre,
                                        char *out_text,
                                        size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
