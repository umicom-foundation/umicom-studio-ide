/* Umicom Studio IDE | Compiler catalogue v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/compiler_catalogue.h"
#include "umicom/compiler/language.h"
#include <stdio.h>
#include <string.h>
static void append_language(char *output,size_t capacity,UmiCompilerLanguage language,bool *first)
{ size_t length = strlen(output); const char *name = umi_compiler_language_name(language); if (length >= capacity) return; (void)snprintf(output + length,capacity - length,"%s%s",*first ? "" : ", ",name); *first = false; }
UmiStatus umi_studio_compiler_catalogue_project(const UmiCompilerRegistry *registry,const char *selected_provider,UmiStudioCompilerCatalogue *out_catalogue)
{
    size_t index; if (registry == NULL || out_catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT; (void)memset(out_catalogue,0,sizeof(*out_catalogue));
    for (index = 0U; index < registry->count && index < UMI_STUDIO_COMPILER_CATALOGUE_MAX; ++index) { const UmiCompilerProvider *provider = &registry->items[index]; UmiStudioCompilerCatalogueRow *row = &out_catalogue->rows[out_catalogue->count++]; UmiCompilerLanguage language; bool first = true; (void)snprintf(row->provider_id,sizeof(row->provider_id),"%s",provider->descriptor.provider_id); (void)snprintf(row->name,sizeof(row->name),"%s",provider->descriptor.display_name); (void)snprintf(row->family,sizeof(row->family),"%s",umi_compiler_family_name(provider->descriptor.family)); (void)snprintf(row->executable,sizeof(row->executable),"%s",provider->descriptor.executable); for (language = UMI_COMPILER_LANGUAGE_C; language <= UMI_COMPILER_LANGUAGE_UAI; language = (UmiCompilerLanguage)(language + 1)) if (umi_compiler_provider_supports(provider,language)) append_language(row->languages,sizeof(row->languages),language,&first); row->available = provider->descriptor.available; row->selected = selected_provider != NULL && strcmp(selected_provider,row->provider_id) == 0; }
    out_catalogue->revision = registry->revision; return UMI_STATUS_OK;
}
const UmiStudioCompilerCatalogueRow *umi_studio_compiler_catalogue_find(const UmiStudioCompilerCatalogue *catalogue,const char *provider_id)
{ size_t index; if (catalogue == NULL || provider_id == NULL) return NULL; for (index = 0U; index < catalogue->count; ++index) if (strcmp(catalogue->rows[index].provider_id,provider_id) == 0) return &catalogue->rows[index]; return NULL; }
