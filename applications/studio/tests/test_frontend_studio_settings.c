/* Umicom Studio IDE Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/studio/frontend_studio_settings.h"
int main(void){UmiStudioWebPlatform *p=NULL;UmiStudioFrontendSettings s=umi_studio_frontend_settings_default();UmiFrontendStudio *f;assert(umi_studio_web_platform_create(&p)==UMI_STATUS_OK);s.port=5173U;(void)strcpy(s.source_root,"applications/studio/web");assert(umi_studio_frontend_settings_apply(p,&s)==UMI_STATUS_OK);f=umi_studio_web_platform_frontend_studio(p);assert(f!=NULL&&f->server.config.port==5173U&&strstr(f->server.base_url,"5173")!=NULL);umi_studio_web_platform_destroy(p);return 0;}
