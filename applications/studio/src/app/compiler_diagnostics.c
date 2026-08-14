/* Umicom Studio IDE | Compiler diagnostics projection v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/compiler_diagnostics.h"
#include <stdio.h>
#include <string.h>
bool umi_studio_compiler_diagnostic_matches(const UmiCompilerDiagnostic *diagnostic,const UmiStudioCompilerDiagnosticFilter *filter)
{ if (diagnostic == NULL) return false; if (filter == NULL) return true; if (diagnostic->severity < filter->minimum_severity) return false; if (filter->file_contains[0] != '\0' && strstr(diagnostic->file,filter->file_contains) == NULL) return false; return filter->code_contains[0] == '\0' || strstr(diagnostic->code,filter->code_contains) != NULL; }
UmiStatus umi_studio_compiler_diagnostics_summarise(const UmiCompilerDiagnosticSet *set,const UmiStudioCompilerDiagnosticFilter *filter,UmiStudioCompilerDiagnosticSummary *out_summary)
{
    size_t index; if (set == NULL || out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT; (void)memset(out_summary,0,sizeof(*out_summary));
    for (index = 0U; index < set->count; ++index) if (umi_studio_compiler_diagnostic_matches(&set->items[index],filter)) { out_summary->visible += 1U; if (set->items[index].severity >= UMI_COMPILER_DIAGNOSTIC_ERROR) out_summary->errors += 1U; else if (set->items[index].severity == UMI_COMPILER_DIAGNOSTIC_WARNING) out_summary->warnings += 1U; else out_summary->notes += 1U; }
    (void)snprintf(out_summary->headline,sizeof(out_summary->headline),"%zu compiler problem(s): %zu error(s), %zu warning(s), %zu note(s)",out_summary->visible,out_summary->errors,out_summary->warnings,out_summary->notes); return UMI_STATUS_OK;
}
