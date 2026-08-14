/* Umicom Studio IDE Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/studio/frontend_studio_views.h"
int main(void){UmiStudioWebPlatform *p=NULL;UmiStudioFrontendCentre c;UmiStudioFrontendView v;assert(umi_studio_web_platform_create(&p)==UMI_STATUS_OK);assert(umi_studio_frontend_centre_init(&c,p)==UMI_STATUS_OK);assert(umi_studio_frontend_view_build(&c,UMI_STUDIO_FRONTEND_VIEW_PREVIEW,&v)==UMI_STATUS_OK);assert(strcmp(v.id,"preview")==0&&strstr(v.summary,"desktop-1440")!=NULL);assert(umi_studio_frontend_view_build(&c,UMI_STUDIO_FRONTEND_VIEW_BUILD,&v)==UMI_STATUS_OK&&v.row_count==1U);umi_studio_web_platform_destroy(p);return 0;}
