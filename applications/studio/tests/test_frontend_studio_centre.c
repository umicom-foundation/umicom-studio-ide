/* Umicom Studio IDE Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/studio/frontend_studio_centre.h"
int main(void){UmiStudioWebPlatform *p=NULL;UmiStudioFrontendCentre c;UmiStudioFrontendCentreSnapshot s;UmiFrontendDocument d={0};uint64_t g=0U;(void)strcpy(d.path,"web/index.html");(void)strcpy(d.media_type,"text/html");(void)strcpy(d.content,"<main>Studio</main>");assert(umi_studio_web_platform_create(&p)==UMI_STATUS_OK);assert(umi_studio_frontend_centre_init(&c,p)==UMI_STATUS_OK);assert(umi_studio_frontend_centre_open_document(&c,&d)==UMI_STATUS_OK);assert(umi_studio_frontend_centre_publish(&c,&g)==UMI_STATUS_OK&&g>1U);assert(umi_studio_frontend_centre_snapshot(&c,&s)==UMI_STATUS_OK&&s.frontend.document_count==1U);umi_studio_web_platform_destroy(p);return 0;}
