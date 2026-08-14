/* Umicom Studio IDE Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/studio/frontend_studio_commands.h"
int main(void){UmiStudioWebPlatform *p=NULL;UmiStudioFrontendCentre c;char m[512];assert(umi_studio_frontend_commands_count()==UMI_STUDIO_FRONTEND_COMMAND_COUNT);assert(umi_studio_frontend_command_at(0U)!=NULL);assert(umi_studio_web_platform_create(&p)==UMI_STATUS_OK);assert(umi_studio_frontend_centre_init(&c,p)==UMI_STATUS_OK);assert(umi_studio_frontend_command_execute(&c,"frontend.preview.start",NULL,m,sizeof(m))==UMI_STATUS_OK);assert(strstr(m,"OK")!=NULL);assert(umi_studio_frontend_command_execute(&c,"frontend.device.select","tablet-768",m,sizeof(m))==UMI_STATUS_OK);assert(umi_studio_frontend_command_execute(&c,"frontend.pane.activate","network",m,sizeof(m))==UMI_STATUS_OK);assert(strcmp(c.active_pane,"network")==0);umi_studio_web_platform_destroy(p);return 0;}
