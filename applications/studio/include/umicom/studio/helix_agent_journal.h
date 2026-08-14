/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/helix_agent_journal.h
 * PURPOSE: Inspect Helix v2 plan, action and evidence records in Studio.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: The reporting helper formats copies of journal entries; it
 * never provides a mutation path into Framework's append-only record. */
#ifndef UMICOM_STUDIO_HELIX_AGENT_JOURNAL_H
#define UMICOM_STUDIO_HELIX_AGENT_JOURNAL_H

#include "umicom/studio/helix_agent_centre.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_studio_helix_agent_journal_count(
    UmiStudioHelixAgentCentre *centre);
int umi_studio_helix_agent_journal_valid(
    UmiStudioHelixAgentCentre *centre);
UmiStatus umi_studio_helix_agent_journal_entry(
    UmiStudioHelixAgentCentre *centre,
    size_t index,
    char *out_text,
    size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
