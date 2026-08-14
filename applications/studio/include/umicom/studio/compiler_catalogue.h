/* Umicom Studio IDE | Compiler catalogue v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_COMPILER_CATALOGUE_H
#define UMICOM_STUDIO_COMPILER_CATALOGUE_H
#include "umicom/compiler/compiler.h"
#define UMI_STUDIO_COMPILER_CATALOGUE_MAX 16U
typedef struct UmiStudioCompilerCatalogueRow { char provider_id[UMI_COMPILER_ID_CAPACITY]; char name[UMI_COMPILER_NAME_CAPACITY]; char family[64U]; char executable[UMI_COMPILER_PATH_CAPACITY]; char languages[UMI_COMPILER_NAME_CAPACITY]; bool available; bool selected; } UmiStudioCompilerCatalogueRow;
typedef struct UmiStudioCompilerCatalogue { UmiStudioCompilerCatalogueRow rows[UMI_STUDIO_COMPILER_CATALOGUE_MAX]; size_t count; uint64_t revision; } UmiStudioCompilerCatalogue;
UmiStatus umi_studio_compiler_catalogue_project(const UmiCompilerRegistry *registry,const char *selected_provider,UmiStudioCompilerCatalogue *out_catalogue);
const UmiStudioCompilerCatalogueRow *umi_studio_compiler_catalogue_find(const UmiStudioCompilerCatalogue *catalogue,const char *provider_id);
#endif
