/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/declarative.h
 *
 * PURPOSE:
 *   Expose Studio use cases for parsing, validating, formatting and compiling toolkit-neutral .umiapp documents.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Studio-facing contract composes reusable Framework capabilities.
 * Generic designer and declarative logic remains owned by Umicom Framework.
 */

#ifndef UMICOM_STUDIO_DECLARATIVE_H
#define UMICOM_STUDIO_DECLARATIVE_H
#include "umicom/umicom.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioDeclarative UmiStudioDeclarative;
typedef struct UmiStudioDeclarativeSnapshot { size_t component_types; UmiDeclVersion schema_version; } UmiStudioDeclarativeSnapshot;
UmiStatus umi_studio_declarative_create(UmiStudioDeclarative **out_service);
void umi_studio_declarative_destroy(UmiStudioDeclarative *service);
UmiStatus umi_studio_declarative_parse(UmiStudioDeclarative *service,const char *source,UmiDeclDocument **out_document,UmiDeclDiagnosticList *diagnostics);
UmiStatus umi_studio_declarative_validate(UmiStudioDeclarative *service,const UmiDeclDocument *document,UmiDeclDiagnosticList *diagnostics);
UmiStatus umi_studio_declarative_compile(UmiStudioDeclarative *service,const UmiDeclDocument *document,UmiDeclApplicationPlan *out_plan,UmiDeclDiagnosticList *diagnostics);
UmiStatus umi_studio_declarative_format(UmiStudioDeclarative *service,const char *source,char *out_text,size_t capacity,UmiDeclDiagnosticList *diagnostics);
UmiStatus umi_studio_declarative_snapshot(const UmiStudioDeclarative *service,UmiStudioDeclarativeSnapshot *out_snapshot);
UmiDeclComponentRegistry *umi_studio_declarative_components(UmiStudioDeclarative *service);
const UmiDeclSchema *umi_studio_declarative_schema(const UmiStudioDeclarative *service);
#ifdef __cplusplus
}
#endif
#endif
