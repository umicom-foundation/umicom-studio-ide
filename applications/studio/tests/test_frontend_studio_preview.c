/* Umicom Studio IDE Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/studio/frontend_studio_preview.h"
int main(void){UmiStudioWebPlatform *p=NULL;UmiFrontendStudio *f;assert(umi_studio_web_platform_create(&p)==UMI_STATUS_OK);assert(umi_studio_frontend_preview_start(p)==UMI_STATUS_OK);assert(umi_studio_frontend_preview_select_device(p,"phone-390")==UMI_STATUS_OK);assert(umi_studio_frontend_preview_navigate(p,"http://127.0.0.1:4310/settings")==UMI_STATUS_OK);assert(umi_studio_frontend_preview_reload(p)==UMI_STATUS_OK);f=umi_studio_web_platform_frontend_studio(p);assert(strcmp(f->preview.device.id,"phone-390")==0&&f->preview.reload_count==1U);umi_studio_web_platform_destroy(p);return 0;}
