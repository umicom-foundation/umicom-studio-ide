/* Umicom Studio IDE | Compiler settings v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/compiler_settings.h"
#include <stdio.h>
#include <string.h>
void umi_studio_compiler_settings_defaults(UmiStudioCompilerSettings *settings)
{ if (settings == NULL) return; (void)memset(settings,0,sizeof(*settings)); (void)snprintf(settings->default_profile,sizeof(settings->default_profile),"host-debug"); (void)snprintf(settings->build_directory,sizeof(settings->build_directory),"build/umicom"); (void)snprintf(settings->compile_database_path,sizeof(settings->compile_database_path),"build/umicom/compile_commands.json"); settings->auto_detect = true; settings->auto_save_lockfile = true; settings->reveal_diagnostics = true; settings->parallel_build = true; settings->parallel_jobs = 0U; settings->revision = 1U; }
UmiStatus umi_studio_compiler_settings_validate(const UmiStudioCompilerSettings *settings,char *out_reason,size_t capacity)
{ const char *reason = "Compiler settings are valid"; UmiStatus status = UMI_STATUS_OK; if (settings == NULL || out_reason == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT; if (settings->default_profile[0] == '\0' || settings->build_directory[0] == '\0') { reason = "A default profile and build directory are required"; status = UMI_STATUS_INVALID_STATE; } else if (!settings->parallel_build && settings->parallel_jobs > 1U) { reason = "Parallel jobs require parallel build mode"; status = UMI_STATUS_INVALID_STATE; } (void)snprintf(out_reason,capacity,"%s",reason); return status; }
