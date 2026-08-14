/* Umicom Studio IDE | Umicc integration v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_UMICC_INTEGRATION_H
#define UMICOM_STUDIO_UMICC_INTEGRATION_H
#include "umicom/compiler/compiler.h"
typedef struct UmiStudioUmiccIntegration { char executable[UMI_COMPILER_PATH_CAPACITY]; char version[64U]; char target[UMI_COMPILER_NAME_CAPACITY]; uint32_t language_mask; bool available; bool local_runtime; bool stable_c_abi; uint64_t revision; } UmiStudioUmiccIntegration;
UmiStatus umi_studio_umicc_integration_configure(UmiStudioUmiccIntegration *integration,const char *executable,const char *version,const char *target,bool local_runtime);
UmiCompilerProvider umi_studio_umicc_integration_provider(const UmiStudioUmiccIntegration *integration);
UmiStatus umi_studio_umicc_integration_validate(const UmiStudioUmiccIntegration *integration,char *out_reason,size_t capacity);
#endif
