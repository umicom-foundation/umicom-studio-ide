/* Umicom Studio IDE | Compiler catalogue v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_COMPILER_CATALOGUE_V2_H
#define UMICOM_STUDIO_COMPILER_CATALOGUE_V2_H
#include "umicom/compiler/compiler.h"
#define UMI_STUDIO_COMPILER_CATALOGUE_MAX 16U
typedef struct UmiStudioCompilerCatalogueRowV2 { char provider_id[UMI_COMPILER_ID_CAPACITY]; char name[UMI_COMPILER_NAME_CAPACITY]; char family[64U]; char executable[UMI_COMPILER_PATH_CAPACITY]; char languages[UMI_COMPILER_NAME_CAPACITY]; bool available; bool selected; } UmiStudioCompilerCatalogueRowV2;
typedef struct UmiStudioCompilerCatalogueV2 { UmiStudioCompilerCatalogueRowV2 rows[UMI_STUDIO_COMPILER_CATALOGUE_MAX]; size_t count; uint64_t revision; } UmiStudioCompilerCatalogueV2;
UmiStatus umi_studio_compiler_catalogue_v2_project(const UmiCompilerRegistry *registry,const char *selected_provider,UmiStudioCompilerCatalogueV2 *out_catalogue);
const UmiStudioCompilerCatalogueRowV2 *umi_studio_compiler_catalogue_v2_find(const UmiStudioCompilerCatalogueV2 *catalogue,const char *provider_id);
#endif
