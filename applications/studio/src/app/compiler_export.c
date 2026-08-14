/* Umicom Studio IDE | Compiler export v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/compiler_export.h"
#include <stdio.h>
UmiStatus umi_studio_compiler_export_summary(const UmiCompilerCentreSnapshot *snapshot,char *out_text,size_t capacity)
{ int written; if (snapshot == NULL || out_text == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT; written = snprintf(out_text,capacity,"Umicom Compiler Centre\nProviders: %zu (%zu available)\nProfiles: %zu\nProjects: %zu\nUnits: %zu\nCompilation commands: %zu\nActive profile: %s\n",snapshot->providers,snapshot->available_providers,snapshot->profiles,snapshot->manifests,snapshot->units,snapshot->compilation_commands,snapshot->active_profile_id); return written < 0 || (size_t)written >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK; }
UmiStatus umi_studio_compiler_export_database(const UmiCompilerCentre *centre,char *out_json,size_t capacity) { if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT; return umi_polyglot_compilation_database_export_json(&centre->compilation_database,out_json,capacity); }
