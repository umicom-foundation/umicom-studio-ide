/* Umicom Studio IDE | Compiler Centre views v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_COMPILER_VIEWS_V2_H
#define UMICOM_STUDIO_COMPILER_VIEWS_V2_H
#include <stddef.h>
#include <stdbool.h>
typedef enum UmiStudioCompilerViewRegionV2 { UMI_STUDIO_COMPILER_VIEW_PRIMARY = 1, UMI_STUDIO_COMPILER_VIEW_SECONDARY, UMI_STUDIO_COMPILER_VIEW_BOTTOM } UmiStudioCompilerViewRegionV2;
typedef struct UmiStudioCompilerViewV2 { const char *view_id; const char *title; const char *icon; const char *description; UmiStudioCompilerViewRegionV2 default_region; bool closable; bool movable; bool supports_multiple_instances; } UmiStudioCompilerViewV2;
size_t umi_studio_compiler_views_v2_count(void);
const UmiStudioCompilerViewV2 *umi_studio_compiler_views_v2_at(size_t index);
const UmiStudioCompilerViewV2 *umi_studio_compiler_views_v2_find(const char *view_id);
#endif
