/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 validated settings adapter
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/visual_builder_settings.h"
#include <stdint.h>
UmiStudioVisualBuilderConfig umi_studio_visual_builder_config_default(void){UmiStudioVisualBuilderConfig config;umi_designer_surface_options_init(&config.surface);config.preview_profile=UMI_DESIGNER_PREVIEW_DESKTOP;config.show_generated_source=1;config.auto_refresh_preview=1;return config;}
UmiStatus umi_studio_visual_builder_settings_apply(const UmiSettings *settings,UmiStudioVisualBuilderConfig *config)
{
    int64_t grid=0,profile=0;int snap=0,show_source=0,auto_preview=0;UmiStatus status;
    if(settings==NULL||config==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_settings_get_integer(settings,UMI_STUDIO_SETTING_VISUAL_BUILDER_GRID_SIZE,&grid);
    if(status==UMI_STATUS_OK)status=umi_settings_get_boolean(settings,UMI_STUDIO_SETTING_VISUAL_BUILDER_SNAP,&snap);
    if(status==UMI_STATUS_OK)status=umi_settings_get_integer(settings,UMI_STUDIO_SETTING_VISUAL_BUILDER_PREVIEW_PROFILE,&profile);
    if(status==UMI_STATUS_OK)status=umi_settings_get_boolean(settings,UMI_STUDIO_SETTING_VISUAL_BUILDER_SHOW_SOURCE,&show_source);
    if(status==UMI_STATUS_OK)status=umi_settings_get_boolean(settings,UMI_STUDIO_SETTING_VISUAL_BUILDER_AUTO_PREVIEW,&auto_preview);
    if(status!=UMI_STATUS_OK)return status;
    if(grid<1||grid>256||profile<UMI_DESIGNER_PREVIEW_DESKTOP||profile>UMI_DESIGNER_PREVIEW_PHONE_PORTRAIT)return UMI_STATUS_INVALID_STATE;
    status=umi_designer_surface_set_grid(&config->surface,(unsigned)grid,snap);config->preview_profile=(UmiDesignerPreviewProfile)profile;config->show_generated_source=show_source;config->auto_refresh_preview=auto_preview;return status;
}
