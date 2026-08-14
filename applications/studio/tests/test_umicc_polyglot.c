/* Umicom Studio Batch 58 test | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/polyglot_workspace.h"
#include "umicom/studio/umicc_integration.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
int main(void) { UmiStudioUmiccIntegration umicc; UmiCompilerProjectManifest manifest; UmiCompilerUnitManifest unit = {0}; UmiStudioPolyglotWorkspace *workspace = calloc(1U,sizeof(*workspace)); char reason[256]; assert(workspace != NULL); assert(umi_studio_umicc_integration_configure(&umicc,"umicc","0.1","x86_64-unknown-linux-gnu",true) == UMI_STATUS_OK); assert(umi_studio_umicc_integration_validate(&umicc,reason,sizeof(reason)) == UMI_STATUS_OK); assert(umi_compiler_manifest_init(&manifest,"p","Poly","x86_64-unknown-linux-gnu") == UMI_STATUS_OK); (void)strcpy(unit.unit_id,"main"); (void)strcpy(unit.source,"main.uai"); (void)strcpy(unit.output,"main.o"); unit.language = UMI_COMPILER_LANGUAGE_UAI; assert(umi_compiler_manifest_add_unit(&manifest,&unit) == UMI_STATUS_OK); assert(umi_studio_polyglot_workspace_init(workspace,"w",&manifest) == UMI_STATUS_OK); assert(umi_studio_polyglot_workspace_language_count(workspace) == 1U); free(workspace); return 0; }
