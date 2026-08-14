/* Umicom Studio IDE | Sammy Hegab | Umicom Foundation | MIT */
#include <stdio.h>
#include "umicom/studio/frontend_studio_commands.h"
int main(void){UmiStudioWebPlatform *p=NULL;UmiStudioFrontendCentre c;UmiStudioFrontendCentreSnapshot s;char message[512];UmiStatus st=umi_studio_web_platform_create(&p);if(st==UMI_STATUS_OK)st=umi_studio_frontend_centre_init(&c,p);if(st==UMI_STATUS_OK)st=umi_studio_frontend_command_execute(&c,"frontend.preview.start",NULL,message,sizeof(message));if(st==UMI_STATUS_OK)st=umi_studio_frontend_centre_snapshot(&c,&s);if(st==UMI_STATUS_OK)(void)printf("%s\nPreview: %s (%zu devices)\n",message,s.frontend.preview_url,s.frontend.device_count);umi_studio_web_platform_destroy(p);return st==UMI_STATUS_OK?0:1;}
