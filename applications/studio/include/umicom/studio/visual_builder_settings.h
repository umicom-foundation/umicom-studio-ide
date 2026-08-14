/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 validated settings adapter
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Typed settings produce one reusable frontend-independent builder configuration. */
#ifndef UMICOM_STUDIO_VISUAL_BUILDER_SETTINGS_H
#define UMICOM_STUDIO_VISUAL_BUILDER_SETTINGS_H
#include "umicom/studio/settings.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioVisualBuilderConfig {UmiDesignerSurfaceOptions surface;UmiDesignerPreviewProfile preview_profile;int show_generated_source;int auto_refresh_preview;} UmiStudioVisualBuilderConfig;
UmiStudioVisualBuilderConfig umi_studio_visual_builder_config_default(void);
UmiStatus umi_studio_visual_builder_settings_apply(const UmiSettings *settings,UmiStudioVisualBuilderConfig *config);
#ifdef __cplusplus
}
#endif
#endif
