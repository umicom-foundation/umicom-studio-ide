/* Umicom Studio IDE | Compiler Centre views v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_COMPILER_VIEWS_H
#define UMICOM_STUDIO_COMPILER_VIEWS_H
#include <stddef.h>
#include <stdbool.h>
typedef enum UmiStudioCompilerViewRegion { UMI_STUDIO_COMPILER_VIEW_PRIMARY = 1, UMI_STUDIO_COMPILER_VIEW_SECONDARY, UMI_STUDIO_COMPILER_VIEW_BOTTOM } UmiStudioCompilerViewRegion;
typedef struct UmiStudioCompilerView { const char *view_id; const char *title; const char *icon; const char *description; UmiStudioCompilerViewRegion default_region; bool closable; bool movable; bool supports_multiple_instances; } UmiStudioCompilerView;
size_t umi_studio_compiler_views_count(void);
const UmiStudioCompilerView *umi_studio_compiler_views_at(size_t index);
const UmiStudioCompilerView *umi_studio_compiler_views_find(const char *view_id);
#endif
